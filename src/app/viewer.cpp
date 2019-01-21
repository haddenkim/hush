// clang-format off
#include <glad/glad.h>
#include <imgui.h> // needs to be included before imgui_imp_... files
// clang-format on

#include "viewer.h"
#include "gui/imgui/imgui_impl_glfw.h"
#include "gui/imgui/imgui_impl_opengl3.h"
#include "pipeline/pipeline.h"
#include <cassert>

// window properties
GLFWwindow* Viewer::Window;

Viewer::Viewer(int width, int height, Scene* scene, Camera* camera)
	: m_windowWidth(width)
	, m_windowHeight(height)
	, m_scene(scene)
	, m_camera(camera)
{
	setupWindow();

	// initial settings
	m_renderMode = CONTINUOUS;
}

void Viewer::setupWindow()
{
	// glfw setup
	glfwInit();

	// GL versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create glfw window
	Window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Hush", NULL, NULL);
	if (!Window) {
		printf("GLFW FAILED\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(Window);

	// GL loader
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("GLAD FAILED\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Viewer::addPipeline(Pipeline* pipeline)
{
	m_availablePipelines.push_back(pipeline);
}

void Viewer::selectPipeline(int id)
{
	assert(id < m_availablePipelines.size());

	m_activePipeline = m_availablePipelines[id];
	m_activePipelineId = id;
}

void Viewer::start()
{
	selectPipeline(0);

	// Main loop
	while (!glfwWindowShouldClose(Window)) {
		/* timer */
		bool timeThisFrame = false;
		auto startTime = std::chrono::high_resolution_clock::now();
		float tracerTime;
		float postProcessTime;

		// clear screen
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // not black to troubleshoot
		glClear(GL_COLOR_BUFFER_BIT);

		// set viewport to render dimensions, NOT window dimensions
		glViewport(0, 0, m_activePipeline->m_width, m_activePipeline->m_height);

		/* render scene */
		if (!m_isPaused) {
			timeThisFrame = true;

			m_activePipeline->render();

			if (m_renderMode == ONE_FRAME) {
				m_isPaused = true;
			}
		}

		/* GUI */
		// Poll and handle events (inputs, window resize, etc.)
		auto startGUI = std::chrono::high_resolution_clock::now();
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// process application gui
		guiMainMenu();
		handleKeyPress();

		// CODEHERE - remove demo gui
		ImGui::ShowDemoWindow();

		/* render gui */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (timeThisFrame) {
			m_guiTime = timeFrom(startGUI);
			m_totalTime = timeFrom(startTime);
		}

		glfwSwapBuffers(Window);
	}

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(Window);
	glfwTerminate();
}

float Viewer::timeFrom(std::chrono::time_point<std::chrono::high_resolution_clock> startTime)
{
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
	return (float)microseconds * 0.001f;
}