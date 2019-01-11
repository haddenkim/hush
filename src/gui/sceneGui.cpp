#include "light/light.h"
#include "material/material.h"
#include "scene/mesh.h"
#include "scene/scene.h"

void Scene::guiRead()
{
	ImGui::Text("Center (%f,%f,%f)", m_center.x, m_center.y, m_center.z);
	ImGui::Text("Min Bounds (%f,%f,%f)", m_minBounds.x, m_minBounds.y, m_minBounds.z);
	ImGui::Text("Max Bounds (%f,%f,%f)", m_maxBounds.x, m_maxBounds.y, m_maxBounds.z);

	ImGui::NewLine();
	ImGui::Text("Vertices %d", m_vertexCount);
	ImGui::Text("Triangles %d", m_triCount);
	ImGui::Text("Materials %lu", m_materialList.size() - 1); // -1 to account for the default material
	ImGui::Text("Textures %lu", m_textureList.size());

	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Enabled items");

	/* Mesh */
	if (ImGui::TreeNode("Mesh", "Mesh %lu", m_enabledMeshList.size())) {

		for (Mesh* mesh : m_enabledMeshList) {
			mesh->guiRead();
		}
		ImGui::TreePop();
	}

	/* Lights */
	if (ImGui::TreeNode("Lights", "Lights %lu", m_enabledLightList.size())) {

		for (Light* light : m_enabledLightList) {
			light->guiRead();
		}

		ImGui::TreePop();
	}
}

bool Scene::guiEdit()
{
	bool wasModified = false;

	/* Mesh */
	if (ImGui::TreeNode("Mesh", "Mesh %lu", m_meshList.size())) {

		ImGui::AlignTextToFramePadding();
		if (ImGui::Button("Show All")) {
			enableAllMesh();
			wasModified = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Hide All")) {
			disableAllMesh();
			wasModified = true;
		}

		for (uint i = 0; i < m_meshList.size(); i++) {
			Mesh* mesh = m_meshList[i];
			if (mesh->guiEdit()) {
				mesh->m_isEnabled ? enableMesh(i) : disableMesh(i);
				wasModified = true;
			}
		}
		ImGui::TreePop();
	}

	/* Lights */
	if (ImGui::TreeNode("Lights")) {

		if (ImGui::TreeNode("Geometry Lights", "Geometry Lights %i", m_emissiveMeshCount)) {

			for (uint i = 0; i < m_emissiveMeshCount; i++) {
				Light* light = m_lightList[i];

				if (light->guiEdit()) {
					updateActiveLights();
					wasModified = true;
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Additional Lights")) {

			if (ImGui::Checkbox("Ambient", &m_enableAmbientLight)) {
				wasModified = true;
			}
			if (m_enableAmbientLight) {
				ImGui::Text("\t");
				ImGui::SameLine();
				if (ImGui::SliderFloat("Intensity", &m_ambientIntensity, 0.f, 10.f, "%.1f")) {
					wasModified = true;
				}
			}

			for (uint i = m_emissiveMeshCount; i < m_lightList.size(); i++) {
				Light* light = m_lightList[i];

				if (light->guiEdit()) {
					updateActiveLights();
					wasModified = true;
				}
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	return wasModified;
}