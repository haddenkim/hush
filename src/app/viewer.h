#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include <vector>

class Camera;
class Scene;
class Pipeline;

enum RenderMode : int {
	CONTINUOUS = 0,
	ONE_FRAME = 1
};

class Viewer {
public:
	Viewer(int width, int height, Scene* scene, Camera* camera);

	void addPipeline(Pipeline* pipeline);
	void start();

	static GLFWwindow* Window;

private:
	// window
	int m_windowWidth;
	int m_windowHeight;

	// renderer state
	RenderMode m_renderMode;
	bool m_isPaused;
	float m_guiTime;
	float m_totalTime;

	void selectPipeline(int id);
	int m_activePipelineId;
	Pipeline* m_activePipeline;
	std::vector<Pipeline*> m_availablePipelines;

	// gui
	void guiMainMenu();
	void guiMainMenuScene();
	void guiMainMenuCamera();
	void guiMainMenuPipeline();
	void guiMainMenuFramebuffer();
	void guiMainMenuStats();

	void handleKeyPress();

	//
	Camera* m_camera;
	Scene* m_scene;

	// helpers
	void setupWindow();
	float timeFrom(std::chrono::time_point<std::chrono::high_resolution_clock> startTime);
};
