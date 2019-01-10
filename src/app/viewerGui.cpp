#include "viewer.h"

#include "app/gui/imguiExtensions.h"
#include "camera/camera.h"
#include "denoiser/atrousDenoiser.h"
#include "light/light.h"
#include "light/meshLight.h"
#include "material/material.h"
#include "renderer/glRenderer.h"
#include "renderer/ptRenderer.h"
#include "renderer/renderer.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "texture/texture.h"
#include <imgui.h>
#include <string>

static bool showEditSceneWindow;

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
	if (ImGui::BeginMenu("Scene")) {
		// if (ImGui::MenuItem("Open..", "Ctrl+O")) {
		// 	// printf("Menu->Scene->Open\n");
		// }
		// if (ImGui::MenuItem("Clear", "Ctrl+Z")) {
		// 	// printf("Menu->Scene->Clear\n");
		// }
		ImGui::MenuItem("Edit Scene", NULL, &showEditSceneWindow);

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("Center (%f,%f,%f)", m_scene->m_center.x, m_scene->m_center.y, m_scene->m_center.z);
		ImGui::Text("Min Bounds (%f,%f,%f)", m_scene->m_minBounds.x, m_scene->m_minBounds.y, m_scene->m_minBounds.z);
		ImGui::Text("Max Bounds (%f,%f,%f)", m_scene->m_maxBounds.x, m_scene->m_maxBounds.y, m_scene->m_maxBounds.z);

		ImGui::NewLine();
		ImGui::Text("Vertices %d", m_scene->m_vertexCount);
		ImGui::Text("Triangles %d", m_scene->m_triCount);
		ImGui::Text("Materials %d", m_scene->m_materialList.size() - 1); // -1 to account for the default material
		ImGui::Text("Textures %d", m_scene->m_textureList.size());

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text("Enabled items");

		/* Mesh */
		if (ImGui::TreeNode("Mesh", "Mesh %i", m_scene->m_enabledMeshList.size())) {

			for (Mesh* mesh : m_scene->m_enabledMeshList) {

				if (ImGui::TreeNode(mesh->m_name.c_str())) {
					ImGui::Text("Vertices %d", mesh->m_vertices.size());
					ImGui::Text("Triangles %d", mesh->m_faces.size());

					if (ImGui::TreeNode("Material")) {
						Material* material = mesh->m_material;

						ImGui::Text("name: %s", material->m_name.c_str());

						if (material->m_diffuseTexture) {
							ImGui::Text("diffuse textured %i,%i", material->m_diffuseTexture->m_width, material->m_diffuseTexture->m_height);
						} else {
							ImGui::Text("diffuse %f,%f,%f", material->m_diffuse[0], material->m_diffuse[1], material->m_diffuse[2]);
						}

						ImGui::Text("ambient %f,%f,%f", material->m_ambient[0], material->m_ambient[1], material->m_ambient[2]);
						ImGui::Text("specular %f,%f,%f", material->m_specular[0], material->m_specular[1], material->m_specular[2]);
						ImGui::Text("transmittance %f,%f,%f", material->m_transmittance[0], material->m_transmittance[1], material->m_transmittance[2]);
						ImGui::Text("emission %f,%f,%f", material->m_emission[0], material->m_emission[1], material->m_emission[2]);
						ImGui::Text("shininess %f", material->m_shininess);
						ImGui::Text("refractionIndex %f", material->m_refractionIndex);

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		/* Lights */
		if (ImGui::TreeNode("Lights", "Lights %i", m_scene->m_enabledLightList.size())) {

			for (Light* light : m_scene->m_enabledLightList) {
				if (ImGui::TreeNode(light->m_name.c_str())) {
					ImGui::Text("power %f,%f,%f", light->m_power[0], light->m_power[1], light->m_power[2]);
					ImGui::Text("position %f,%f,%f", light->m_position[0], light->m_position[1], light->m_position[2]);
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::EndMenu();
	}

	// child windows
	if (showEditSceneWindow) {
		guiWindowEditScene();
	}
}
void Viewer::guiMainMenuCamera()
{
	if (ImGui::BeginMenu("Camera")) {
		if (ImGui::MenuItem("Reset")) {
			m_camera->reset();
		}

		ImGui::Separator();
		const char* modes[] = { "Orbit", "Rotate", "Translate" };
		ImGui::Combo("Mode", (int*)&m_camera->m_mode, modes, IM_ARRAYSIZE(modes));

		if (ImGui::InputVec3f("Position", &m_camera->m_position)) {
			m_camera->updateMatrices();
		}
		if (ImGui::InputVec3f("Center", &m_camera->m_center)) {
			m_camera->updateMatrices();
		}
		if (ImGui::InputVec3f("Up", &m_camera->m_up)) {
			m_camera->updateMatrices();
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Camera Controls")) {
			ImGui::Text("All Modes:");
			ImGui::BulletText("Z,X - Zoom");
			ImGui::BulletText("C - Switch Mode");
			ImGui::BulletText("R - Reset");

			ImGui::Text("Orbit & Rotate Mode:");
			ImGui::BulletText("W,S - Pitch");
			ImGui::BulletText("A,D - Yaw");
			ImGui::BulletText("Q,E - Roll");

			ImGui::Text("Translate Mode:");
			ImGui::BulletText("W - Forward");
			ImGui::BulletText("S - Backward");
			ImGui::BulletText("A - Left");
			ImGui::BulletText("D - Right");
			ImGui::BulletText("Q - Up");
			ImGui::BulletText("E - Down");
		}

		ImGui::EndMenu();
	}
}
void Viewer::guiMainMenuRenderer()
{
	if (ImGui::BeginMenu("Renderer")) {
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

		// const char* renderers[] = { "(F1) OpenGl", "Path Tracer", "(F2) A-Trous" };
		const char* renderers[] = { "(F1) OpenGl", "(F2) Path Tracer" };
		if (ImGui::Combo("Renderer", &m_activeRendererId, renderers, IM_ARRAYSIZE(renderers))) {

			selectRenderer(m_activeRendererId);
		}

		ImGui::Separator();
		ImGui::NewLine();

		/* Path Tracer */
		if (m_activeRendererId == 1) {
			ImGui::Text("Path Tracer Settings");
			/* film settings */
			if (ImGui::TreeNode("Film")) {
				ImGui::SliderUint("SPP", &m_ptRenderer->m_samplesPerPixel, 1, 10);

				ImGui::TreePop();
			}

			/* direct light settings */
			if (ImGui::TreeNode("Direct Lighting")) {
				const char* options[] = { "Uniform Sample One", "Uniform Sample All" };
				ImGui::Combo("Strategy", (int*)&m_ptRenderer->m_directLightStrategy, options, IM_ARRAYSIZE(options));
				ImGui::SliderUint("# Samples per Light", &m_ptRenderer->m_samplesPerLight, 1, 10);

				ImGui::TreePop();
			}

			/* indirect light settings */
			if (ImGui::TreeNode("Indirect Lighting")) {
				ImGui::SliderUint("Max Depth", &m_ptRenderer->m_maxDepth, 0, 10);
				ImGui::SliderFloat("Min Contribution", &m_ptRenderer->m_minContribution, 0.f, 1.f, "%.2f");

				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::NewLine();

			ImGui::Text("Post Process Settings");
			const char* options[] = { "None", "Atrous" };
			if (ImGui::Combo("Strategy", (int*)&m_ptRenderer->m_denoiseStrategy, options, IM_ARRAYSIZE(options))) {
				m_ptRenderer->selectDenoiser(m_ptRenderer->m_denoiseStrategy);
			}

			if (m_ptRenderer->m_denoiseStrategy == ATROUS) {
				auto& atrousDen = m_ptRenderer->m_atrousDenoiser;
				ImGui::SliderUint("Iterations", &atrousDen.m_filterIterations, 1, 10);
				ImGui::SliderFloat("Sigma Color", &atrousDen.m_colorSigma, 0.01f, 10.f, "%.2f");
				ImGui::SliderFloat("Sigma Position", &atrousDen.m_positionSigma, 0.01f, 10.f, "%.2f");
				ImGui::SliderFloat("Sigma Normal", &atrousDen.m_normalSigma, 0.01f, 10.f, "%.2f");
				ImGui::Checkbox("Multiply by Diffuse?", &atrousDen.m_useOptionalDiffuse);
			}
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
				ImGui::TextDisabled(m_activeRenderer->m_framebufferNames[n].c_str());
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

void Viewer::guiWindowEditScene()
{
	ImGui::Begin("Edit Scene", &showEditSceneWindow);

	/* Mesh */
	if (ImGui::TreeNode("Mesh", "Mesh %i", m_scene->m_meshList.size())) {

		ImGui::AlignTextToFramePadding();
		if (ImGui::Button("Show All")) {
			m_scene->enableAllMesh();
		}
		ImGui::SameLine();
		if (ImGui::Button("Hide All")) {
			m_scene->disableAllMesh();
		}

		for (uint i = 0; i < m_scene->m_meshList.size(); i++) {
			Mesh* mesh = m_scene->m_meshList[i];
			if (ImGui::Checkbox(mesh->m_name.c_str(), &mesh->m_isEnabled)) {
				mesh->m_isEnabled ? m_scene->enableMesh(i) : m_scene->disableMesh(i);
			}
		}
		ImGui::TreePop();
	}

	/* Lights */
	if (ImGui::TreeNode("Lights")) {

		if (ImGui::TreeNode("Geometry Lights", "Geometry Lights %i", m_scene->m_emissiveMeshCount)) {

			for (uint i = 0; i < m_scene->m_emissiveMeshCount; i++) {
				Light* light = m_scene->m_lightList[i];

				if (ImGui::Checkbox(light->m_name.c_str(), &light->m_isEnabled)) {
					light->m_isEnabled ? m_scene->enableLight(i) : m_scene->disableLight(i);
				}

				ImGui::Text("\tpower %f,%f,%f", light->m_power[0], light->m_power[1], light->m_power[2]);
				ImGui::Text("\tposition %f,%f,%f", light->m_position[0], light->m_position[1], light->m_position[2]);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Additional Lights")) {

			ImGui::Checkbox("Ambient", &m_scene->m_enableAmbientLight);
			ImGui::Text("\t");
			ImGui::SameLine();
			ImGui::SliderFloat("Intensity", &m_scene->m_ambientIntensity, 0.f, 10.f, "%.1f");

			for (uint i = m_scene->m_emissiveMeshCount; i < m_scene->m_lightList.size(); i++) {
				Light* light = m_scene->m_lightList[i];

				if (ImGui::Checkbox(light->m_name.c_str(), &light->m_isEnabled)) {
					light->m_isEnabled ? m_scene->enableLight(i) : m_scene->disableLight(i);
				}

				ImGui::Text("\tpower %f,%f,%f", light->m_power[0], light->m_power[1], light->m_power[2]);
				ImGui::Text("\tposition %f,%f,%f", light->m_position[0], light->m_position[1], light->m_position[2]);
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	ImGui::End();
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