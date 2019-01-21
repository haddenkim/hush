#pragma once

#include "common.h"
#include "gui/iGuiEditable.h"
#include <vector>

#define MOVE_SPEED 0.2f
#define ZOOM_SPEED 0.2f

class Scene;

class Camera : public IGuiEditable {
public:
	// clang-format off
	enum Mode { Orbit = 0, Rotate = 1, Translate = 2};
	enum Direction { Left, Right, Forward, Backward, Up, Down };
	enum Orientation { YawPos, YawNeg, PitchPos, PitchNeg, RollPos, RollNeg};
	// clang-format on

	/* Constructors */
	Camera(int width, int height, float fovY, Scene* scene);
	Camera(int width, int height, Point3f position, Point3f center, Vec3f up, float fovY, Scene* scene);

	/* UI */
	bool guiEdit();

	void reset();
	void setBase(Point3f position, Point3f center, Vec3f up);
	void switchMode();
	void switchMode(Mode mode);
	void zoom(bool out, float speed = ZOOM_SPEED);
	void move(Direction direction, float speed = MOVE_SPEED);
	void adjustOrbit(Orientation orientation, float speed = MOVE_SPEED);
	void adjustAttitude(Orientation orientation, float speed = MOVE_SPEED);
	void translate(Direction direction, float speed = MOVE_SPEED);

	void updateMatrices();

	/* data, state */
	// base properties
	int m_width;  // of render screen
	int m_height; // of render screen
	Point3f m_position;
	Point3f m_center;
	Vec3f m_up;
	float m_fovY; // radians

	// anchor for resetting
	Point3f m_positionOriginal;
	Point3f m_centerOriginal;
	Vec3f m_upOriginal;

	// derived properties
	float m_fovScale;
	float m_invWidth;
	float m_invHeight;
	Mat4 m_cameraToWorld;
	Mat4 m_projection;
	Mat4 m_viewProjection;

	// path tracing buffer
	std::vector<Vec3f> m_directionBuffer;

	// other GUI properties
	Mode m_mode;
	Scene* m_scene;

private:
	// helper
	void updateProjectionMatrix(float fov, float aspect, float near, float far);
	Mat4 buildRotationMatrix(Orientation orientation, float speed);
	void updateDirectionBuffer();
};