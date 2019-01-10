#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include <vector>

class Camera;
class Scene;
class Renderer;
class GlRenderer;
class PtRenderer;

enum RenderMode: int {
	CONTINUOUS = 0,
	ONE_FRAME = 1
};

class Viewer {
public:
	Viewer(int width, int height, Camera* camera, Scene* scene);

	void start();

private:
	// window
	static GLFWwindow* Window;
	int m_windowWidth;
	int m_windowHeight;

	// renderer state
	RenderMode m_renderMode;
	bool m_isPaused;
	float m_guiTime;
	float m_tracerTime;
	float m_postProcessTime;
	float m_totalTime;

	void selectRenderer(int id);
	int m_activeRendererId;
	Renderer* m_activeRenderer;
	GlRenderer* m_glRenderer;
	PtRenderer* m_ptRenderer;

	// gui
	void guiMainMenu();
	void guiMainMenuScene();
	void guiMainMenuCamera();
	void guiMainMenuRenderer();
	void guiMainMenuFramebuffer();
	void guiMainMenuStats();
	void guiWindowEditScene();

	void handleKeyPress();

	//
	Camera* m_camera;
	Scene* m_scene;

	// helpers
	void init();
	float timeFrom(std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> startTime);
};
