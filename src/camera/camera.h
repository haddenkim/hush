#pragma once

#include "common.h"

#define MOVE_SPEED 0.2f
#define ZOOM_SPEED 0.2f

class Camera {
public:
	// clang-format off
	enum Mode { Orbit = 0, Rotate = 1, Translate = 2};
	enum Direction { Left, Right, Forward, Backward, Up, Down };
	enum Orientation { YawPos, YawNeg, PitchPos, PitchNeg, RollPos, RollNeg};
	// clang-format on

	Camera(int width, int height, Point3f position, Point3f center, Vec3f up, float fovY);

	// movement
	void reset();
	void switchMode();
	void switchMode(Mode mode);
	void zoom(bool out, float speed = ZOOM_SPEED);
	void move(Direction direction, float speed = MOVE_SPEED);
	void adjustOrbit(Orientation orientation, float speed = MOVE_SPEED);
	void adjustAttitude(Orientation orientation, float speed = MOVE_SPEED);
	void translate(Direction direction, float speed = MOVE_SPEED);
	void updateMatrices();

	// base properties
	int m_width;  // of screen
	int m_height; // of screen
	Point3f m_position;
	Point3f m_center;
	Vec3f m_up;
	float m_fovY; // radians

	// for resetting
	Point3f m_positionOriginal;
	Point3f m_centerOriginal;
	Vec3f m_upOriginal;

	// UI
	Mode m_mode;

	// derived properties
	Mat4 m_cameraToWorld;
	Mat4 m_projection;
	Mat4 m_viewProjection;

private:
	// helper
	void updateProjectionMatrix(float fov, float aspect, float near, float far);
	Mat4 buildRotationMatrix(Orientation orientation, float speed);
};