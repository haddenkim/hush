#include "app/viewer.h"
#include "camera/camera.h"
#include "pipeline/pipeline.h"
#include "scene/scene.h"
#include <imgui.h>
#include <string>

#include "gui/iGuiEditable.h"

void Viewer::guiMainMenu()
{
	if (ImGui::BeginMainMenuBar()) {
		guiMainMenuScene();
		guiMainMenuCamera();
		guiMainMenuPipeline();
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

void Viewer::guiMainMenuPipeline()
{
	if (ImGui::BeginMenu("Pipeline")) {

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

		// Pipeline
		const char* pipelines[] = { "(F1) OpenGl", "(F2) Path Tracer" };
		if (ImGui::Combo("Pipeline", &m_activePipelineId, pipelines, IM_ARRAYSIZE(pipelines))) {
			selectPipeline(m_activePipelineId);
		}

		ImGui::Separator();
		ImGui::NewLine();

		m_activePipeline->guiEdit();

		ImGui::EndMenu();
	}
}

void Viewer::guiMainMenuFramebuffer()
{
	if (ImGui::BeginMenu("Display")) {

		for (uint i = 0; i < m_activePipeline->m_buffers.size(); i++) {
			if (ImGui::Selectable(PipelineIONames[m_activePipeline->m_bufferTypes[i]], m_activePipeline->m_displayedBufferIndex == i)) {
				m_activePipeline->selectDrawBuffer(i);
			}
		}

		ImGui::EndMenu();
	}
}

void Viewer::guiMainMenuStats()
{
	static bool showStatsWindow = false;
	if (ImGui::BeginMenu("Stats")) {

		ImGui::Text("Last Frame");
		// ImGui::Text("\tTracer time: %.1f ms", m_tracerTime);
		// ImGui::Text("\tPost Process time: %.1f ms", m_postProcessTime);

		ImGui::Text("\tGUI time: %.1f ms", m_guiTime);
		ImGui::Text("\tTotal time: %.1f ms", m_totalTime);

		ImGui::Separator();
		ImGui::MenuItem("Show in window", NULL, &showStatsWindow);

		ImGui::EndMenu();
	}

	if (showStatsWindow) {
		ImGui::Begin("Stats", &showStatsWindow, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("Last Frame");
		// ImGui::Text("\tTracer time: %.1f ms", m_tracerTime);
		// ImGui::Text("\tPost Process time: %.1f ms", m_postProcessTime);

		ImGui::Text("\tGUI time: %.1f ms", m_guiTime);
		ImGui::Text("\tTotal time: %.1f ms", m_totalTime);

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

	/* Pipeline selection */
	if (io.KeysDown[GLFW_KEY_SPACE] && io.KeysDownDuration[GLFW_KEY_SPACE] == 0.0f) {
		m_isPaused = !m_isPaused;
	}
	if (io.KeysDown[GLFW_KEY_F1] && io.KeysDownDuration[GLFW_KEY_F1] == 0.0f) {
		selectPipeline(0);
	}
	if (io.KeysDown[GLFW_KEY_F2] && io.KeysDownDuration[GLFW_KEY_F2] == 0.0f) {
		selectPipeline(1);
	}
}