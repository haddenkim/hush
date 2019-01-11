#include "app/viewer.h"
#include "camera/camera.h"
#include "renderer/ptRenderer.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include <imgui.h>
#include <string>

#include "gui/iGuiEditable.h"

void Viewer::guiMainMenu()
{
	if (ImGui::BeginMainMenuBar()) {
		guiMainMenuScene();
		guiMainMenuCamera();
		guiMainMenuRenderer();
		guiMainMenuFramebuffer();
		guiMainMenuStats();
		ImGui::EndMainMenuBar();
	}
}
void Viewer::guiMainMenuScene()
{
	static bool showEditSceneWindow = false;

	if (ImGui::BeginMenu("Scene")) {
		ImGui::MenuItem("Edit Scene", NULL, &showEditSceneWindow);

		ImGui::Separator();
		ImGui::NewLine();

		m_scene->guiRead();

		ImGui::EndMenu();
	}

	// child windows
	if (showEditSceneWindow) {
		ImGui::Begin("Edit Scene", &showEditSceneWindow);
		m_scene->guiEdit();
		ImGui::End();
	}
}

void Viewer::guiMainMenuCamera()
{
	if (ImGui::BeginMenu("Camera")) {
		m_camera->guiEdit();
		
		ImGui::EndMenu();
	}
}
void Viewer::guiMainMenuRenderer()
{
	if (ImGui::BeginMenu("Renderer")) {

		// render mode & framerate
		const char* modes[] = { "Continuous", "One Frame" };
		if (ImGui::Combo("Mode", (int*)&m_renderMode, modes, IM_ARRAYSIZE(modes))) {
		}

		if (m_renderMode == CONTINUOUS) {
			ImGui::MenuItem("Pause", "Space", &m_isPaused);
		}
		if (m_renderMode == ONE_FRAME) {
			// ImGui::MenuItem("Next Frame", "Space", &m_isPaused);
			if (ImGui::MenuItem("Next Frame", "Space", false)) {
				m_isPaused = !m_isPaused;
			}
		}

		ImGui::Separator();
		ImGui::NewLine();

		// Renderer
		const char* renderers[] = { "(F1) OpenGl", "(F2) Path Tracer" };
		if (ImGui::Combo("Renderer", &m_activeRendererId, renderers, IM_ARRAYSIZE(renderers))) {
			selectRenderer(m_activeRendererId);
		}

		ImGui::Separator();
		ImGui::NewLine();

		// Path tracer
		if (m_activeRendererId == 1) {
			m_ptRenderer->guiEdit();

		}

		ImGui::EndMenu();
	}
}
void Viewer::guiMainMenuFramebuffer()
{
	if (ImGui::BeginMenu("Framebuffer")) {
		for (int n = 0; n < m_activeRenderer->m_framebufferTextures.size(); n++) {
			if (m_activeRenderer->m_framebufferAllowed[n]) {
				if (ImGui::Selectable(m_activeRenderer->m_framebufferNames[n].c_str(), m_activeRenderer->m_displayFramebuffer == n))
					m_activeRenderer->m_displayFramebuffer = n;

			} else {
				ImGui::TextDisabled("%s", m_activeRenderer->m_framebufferNames[n].c_str());
			}
		}

		ImGui::EndMenu();
	}
}
void Viewer::guiMainMenuStats()
{
	static bool showStatsWindow = false;
	if (ImGui::BeginMenu("Stats")) {

		ImGui::Text("GUI time: %.1f ms", m_guiTime);
		ImGui::Text("Tracer time: %.1f ms", m_tracerTime);
		ImGui::Text("Post Process time: %.1f ms", m_postProcessTime);
		ImGui::Text("Total time: %.1f ms", m_totalTime);

		ImGui::Separator();
		ImGui::MenuItem("Show in window", NULL, &showStatsWindow);

		ImGui::EndMenu();
	}

	if (showStatsWindow) {
		ImGui::Begin("Stats", &showStatsWindow, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("GUI time: %.1f ms", m_guiTime);
		ImGui::Text("Tracer time: %.1f ms", m_tracerTime);
		ImGui::Text("Post Process time: %.1f ms", m_postProcessTime);
		ImGui::Text("Total time: %.1f ms", m_totalTime);
		ImGui::End();
	}
}

void Viewer::handleKeyPress()
{
	auto io = ImGui::GetIO();

	/* Camera controls */
	if (io.KeysDown[GLFW_KEY_R]) {
		m_camera->reset();
	}
	if (io.KeysDown[GLFW_KEY_A]) {
		m_camera->move(Camera::Direction::Left);
	}
	if (io.KeysDown[GLFW_KEY_D]) {
		m_camera->move(Camera::Direction::Right);
	}
	if (io.KeysDown[GLFW_KEY_W]) {
		m_camera->move(Camera::Direction::Forward);
	}
	if (io.KeysDown[GLFW_KEY_S]) {
		m_camera->move(Camera::Direction::Backward);
	}
	if (io.KeysDown[GLFW_KEY_Q]) {
		m_camera->move(Camera::Direction::Up);
	}
	if (io.KeysDown[GLFW_KEY_E]) {
		m_camera->move(Camera::Direction::Down);
	}
	if (io.KeysDown[GLFW_KEY_Z]) {
		m_camera->zoom(false);
	}
	if (io.KeysDown[GLFW_KEY_X]) {
		m_camera->zoom(true);
	}
	if (io.KeysDown[GLFW_KEY_C] && io.KeysDownDuration[GLFW_KEY_C] == 0.0f) {
		m_camera->switchMode();
	}

	/* Renderer controls */
	if (io.KeysDown[GLFW_KEY_SPACE] && io.KeysDownDuration[GLFW_KEY_SPACE] == 0.0f) {
		m_isPaused = !m_isPaused;
	}
	if (io.KeysDown[GLFW_KEY_F1] && io.KeysDownDuration[GLFW_KEY_F1] == 0.0f) {
		selectRenderer(0);
	}
	if (io.KeysDown[GLFW_KEY_F2] && io.KeysDownDuration[GLFW_KEY_F2] == 0.0f) {
		selectRenderer(1);
	}

	/* Framebuffer display */
	if (io.KeysDown[GLFW_KEY_0] && io.KeysDownDuration[GLFW_KEY_0] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 0;
	}
	if (io.KeysDown[GLFW_KEY_1] && io.KeysDownDuration[GLFW_KEY_1] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 1;
	}
	if (io.KeysDown[GLFW_KEY_2] && io.KeysDownDuration[GLFW_KEY_2] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 2;
	}
	if (io.KeysDown[GLFW_KEY_3] && io.KeysDownDuration[GLFW_KEY_3] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 3;
	}
	if (io.KeysDown[GLFW_KEY_4] && io.KeysDownDuration[GLFW_KEY_4] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 4;
	}
	if (io.KeysDown[GLFW_KEY_5] && io.KeysDownDuration[GLFW_KEY_5] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 5;
	}
	if (io.KeysDown[GLFW_KEY_6] && io.KeysDownDuration[GLFW_KEY_6] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 6;
	}
	if (io.KeysDown[GLFW_KEY_7] && io.KeysDownDuration[GLFW_KEY_7] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 7;
	}
	if (io.KeysDown[GLFW_KEY_8] && io.KeysDownDuration[GLFW_KEY_8] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 8;
	}
	if (io.KeysDown[GLFW_KEY_9] && io.KeysDownDuration[GLFW_KEY_9] == 0.0f) {
		m_activeRenderer->m_displayFramebuffer = 9;
	}
}