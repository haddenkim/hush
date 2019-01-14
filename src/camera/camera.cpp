#include "camera.h"
#include "scene/scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define MAX_ZOOM 100
#define MIN_ZOOM 0.1

Camera::Camera(int width, int height, float fovY, Scene* scene)
	: m_width(width)
	, m_height(height)
	, m_fovY(fovY)
	, m_mode(Orbit)
	, m_scene(scene)
{
	m_projection = glm::perspective(fovY, (float)width / (float)height, 0.01f, 1000.0f);

	setBase(m_scene->m_center + (m_scene->m_maxBounds - m_scene->m_center) * 3.0f,
			m_scene->m_center,
			Vec3f(0.f, 1.f, 0.f));
}

Camera::Camera(int width, int height, Point3f position, Point3f center, Vec3f up, float fovY, Scene* scene)
	: m_width(width)
	, m_height(height)
	, m_fovY(fovY)
	, m_mode(Orbit)
	, m_scene(scene)
{
	// CODEHERE - determine a reasonable zFar
	m_projection = glm::perspective(fovY, (float)width / (float)height, 0.01f, 1000.0f);

	setBase(position, center, up);
}

void Camera::reset()
{
	m_position = m_positionOriginal;
	m_center = m_centerOriginal;
	m_up = m_upOriginal;
	m_mode = Orbit;

	updateMatrices();
}

void Camera::setBase(Point3f position, Point3f center, Vec3f up)
{
	m_position = m_positionOriginal = position;
	m_center = m_centerOriginal = center;
	m_up = m_upOriginal = up;

	updateMatrices();
}

void Camera::switchMode()
{
	int modeInt = m_mode;
	modeInt++;
	if (modeInt > 2)
		modeInt = 0;
	m_mode = static_cast<Mode>(modeInt);
}

void Camera::switchMode(Mode mode)
{
	m_mode = mode;
}

void Camera::move(Direction direction, float speed)
{
	switch (m_mode) {
	case Orbit:
		adjustOrbit((Orientation)direction, speed);
		break;

	case Rotate:
		adjustAttitude((Orientation)direction, speed);
		break;

	case Translate:
		translate(direction, speed);
		break;

	default:
		break;
	}
}

void Camera::zoom(bool out, float speed)
{
	auto look = normalize(m_center - m_position);
	auto dis = distance(m_center, m_position);
	auto newDis = dis + (out ? speed : -speed);

	// clamp distance
	if (newDis < MIN_ZOOM) {
		newDis = MIN_ZOOM;
	}
	if (newDis > MAX_ZOOM) {
		newDis = MAX_ZOOM;
	}

	if (m_mode == Orbit) {
		m_position = m_center - newDis * look;

	} else {
		m_center = m_position + newDis * look;
	}

	updateMatrices();
}

void Camera::adjustOrbit(Orientation orientation, float speed)
{
	auto rotationMatrix = buildRotationMatrix(orientation, speed);

	// modify camera properties
	// yaw -> look, eye
	// pitch -> look, up, eye
	// roll -> up
	switch (orientation) {
	case Camera::Orientation::YawPos:
	case Camera::Orientation::YawNeg: {
		auto look = normalize(m_center - m_position);
		auto dis = distance(m_center, m_position);
		look = rotationMatrix * glm::vec4(look, 0.0f);
		m_position = m_center - dis * look;
		break;
	}

	case Camera::Orientation::PitchPos:
	case Camera::Orientation::PitchNeg: {
		auto look = normalize(m_center - m_position);
		auto dis = distance(m_center, m_position);
		look = rotationMatrix * glm::vec4(look, 0.0f);
		m_position = m_center - dis * look;
		m_up = rotationMatrix * glm::vec4(m_up, 0.0f);
		break;
	}

	case Camera::Orientation::RollPos:
	case Camera::Orientation::RollNeg:
		m_up = rotationMatrix * glm::vec4(m_up, 0.0f);
		break;
	}

	// update view matrix
	updateMatrices();
}

void Camera::adjustAttitude(Orientation orientation, float speed)
{
	auto rotationMatrix = buildRotationMatrix(orientation, -speed);

	// modify camera properties
	// yaw -> look, center
	// pitch -> look, up, center
	// roll -> up
	switch (orientation) {
	case Camera::Orientation::YawPos:
	case Camera::Orientation::YawNeg: {
		auto look = normalize(m_center - m_position);
		auto dis = distance(m_center, m_position);
		look = rotationMatrix * glm::vec4(look, 0.0f);
		m_center = m_position + dis * look;
		break;
	}

	case Camera::Orientation::PitchPos:
	case Camera::Orientation::PitchNeg: {
		auto look = normalize(m_center - m_position);
		auto dis = distance(m_center, m_position);
		look = rotationMatrix * glm::vec4(look, 0.0f);
		m_center = m_position + dis * look;
		m_up = rotationMatrix * glm::vec4(m_up, 0.0f);
		break;
	}

	case Camera::Orientation::RollPos:
	case Camera::Orientation::RollNeg:
		m_up = rotationMatrix * glm::vec4(m_up, 0.0f);
		break;
	}

	// update view matrix
	updateMatrices();
}

void Camera::translate(Direction direction, float speed)
{
	Vec3f dir;
	switch (direction) {
	case Left:
		dir = -m_cameraToWorld[0];
		break;
	case Right:
		dir = m_cameraToWorld[0];
		break;

	case Forward:
		dir = -m_cameraToWorld[2];
		break;
	case Backward:
		dir = m_cameraToWorld[2];
		break;

	case Up:
		dir = m_cameraToWorld[1];
		break;
	case Down:
		dir = -m_cameraToWorld[1];
		break;

	default:
		return;
	}

	m_position += dir * speed;
	m_center += dir * speed;

	updateMatrices();
}

void Camera::updateMatrices()
{
	auto m_view = glm::lookAt(m_position, m_center, m_up);
	m_cameraToWorld = glm::inverse(m_view);
	m_viewProjection = m_projection * m_view;
}

Mat4 Camera::buildRotationMatrix(Orientation orientation, float speed)
{
	// determine angle and axis of rotation
	float angleRotation;
	Vec3f axisRotation;

	switch (orientation) {
	case Camera::Orientation::YawPos:
		angleRotation = -speed;
		axisRotation = m_cameraToWorld[1];
		break;

	case Camera::Orientation::YawNeg:
		angleRotation = speed;
		axisRotation = m_cameraToWorld[1];
		break;

	case Camera::Orientation::PitchPos:
		angleRotation = -speed;
		axisRotation = m_cameraToWorld[0];
		break;

	case Camera::Orientation::PitchNeg:
		angleRotation = speed;
		axisRotation = m_cameraToWorld[0];
		break;

	case Camera::Orientation::RollPos:
		angleRotation = speed;
		axisRotation = m_cameraToWorld[2];
		break;

	case Camera::Orientation::RollNeg:
		angleRotation = -speed;
		axisRotation = m_cameraToWorld[2];
		break;
	}

	return glm::rotate(angleRotation, axisRotation);
}
