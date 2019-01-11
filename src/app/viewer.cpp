// clang-format off
#include <glad/glad.h>
#include <imgui.h> // needs to be included before imgui_imp_... files
// clang-format on

#include "viewer.h"
#include <stdio.h>
#include <stdlib.h>
#include "gui/imgui/imgui_impl_glfw.h"
#include "gui/imgui/imgui_impl_opengl3.h"

#include "renderer/glRenderer.h"
#include "renderer/ptRenderer.h"

// window properties
GLFWwindow* Viewer::Window;

Viewer::Viewer(int width, int height, Camera* camera, Scene* scene)
	: m_windowWidth(width)
	, m_windowHeight(height)
	, m_camera(camera)
	, m_scene(scene)
{
	init();

	m_glRenderer = new GlRenderer(m_scene, m_camera); // 0
	m_ptRenderer = new PtRenderer(m_scene, m_camera); // 1

	m_renderMode = CONTINUOUS;
	selectRenderer(0);
}

void Viewer::init()
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

void Viewer::selectRenderer(int id)
{
	switch (id) {
	case 0:
		m_activeRendererId = id;
		m_activeRenderer = m_glRenderer;
		break;

	case 1:
		m_activeRendererId = id;
		m_activeRenderer = m_ptRenderer;
		break;

	default:
		break;
	}
}

void Viewer::start()
{
	// Main loop
	while (!glfwWindowShouldClose(Window)) {
		/* timer */
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

		// clear screen
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // not black to troubleshoot
		glClear(GL_COLOR_BUFFER_BIT);

		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* handle gui input */
		guiMainMenu();
		handleKeyPress();

		// CODEHERE - remove demo gui
		ImGui::ShowDemoWindow();
		m_guiTime = timeFrom(startTime);

		// set viewport to render dimensions, NOT window dimensions
		glViewport(0, 0, m_activeRenderer->m_width, m_activeRenderer->m_height);

		/* render scene */
		if (!m_isPaused) {

			auto startRender3d = std::chrono::high_resolution_clock::now();
			m_activeRenderer->render3d();
			m_tracerTime = timeFrom(startRender3d);

			auto startRenderPP = std::chrono::high_resolution_clock::now();
			m_activeRenderer->renderPostProcess();
			m_postProcessTime = timeFrom(startRenderPP);

			if (m_renderMode == ONE_FRAME) {
				m_isPaused = true;
			}
		}

		/* render to final window's framebuffer */
		// collect window size - for resizing and mac retina
		int viewPortW, viewPortH;
		glfwGetFramebufferSize(Window, &viewPortW, &viewPortH);
		int viewPortX = (viewPortW - viewPortH) / 2;
		glViewport(viewPortX, 0, viewPortH, viewPortH);

		m_activeRenderer->renderFramebuffer();

		m_totalTime = timeFrom(startTime);

		/* render gui */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
}