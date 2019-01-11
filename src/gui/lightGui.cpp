#include "light/meshLight.h"
#include "light/pointLight.h"
#include "scene/mesh.h"

void PointLight::guiRead() const
{
	if (ImGui::TreeNode(m_name.c_str())) {
		ImGui::Text("intensity %f,%f,%f", m_intensity[0], m_intensity[1], m_intensity[2]);
		ImGui::Text("position %f,%f,%f", m_position[0], m_position[1], m_position[2]);
		ImGui::TreePop();
	}
}

bool PointLight::guiEdit()
{
	bool wasModified = false;

	if (ImGui::Checkbox(m_name.c_str(), &m_isEnabled)) {
		wasModified = true;
	}

	ImGui::Text("\tpower %f,%f,%f", m_power[0], m_power[1], m_power[2]);
	ImGui::Text("\tposition %f,%f,%f", m_position[0], m_position[1], m_position[2]);

	return wasModified;
}

void MeshLight::guiRead() const
{
	if (ImGui::TreeNode(m_name.c_str())) {
		ImGui::Text("radiance %f,%f,%f", m_radiance[0], m_radiance[1], m_radiance[2]);

		ImGui::Text("mesh name: %s", m_mesh->m_name.c_str());
		ImGui::Text("mesh center %f,%f,%f", m_mesh->m_center[0], m_mesh->m_center[1], m_mesh->m_center[2]);
		ImGui::TreePop();
	}
}

bool MeshLight::guiEdit()
{
	bool wasModified = false;

	if (ImGui::Checkbox(m_name.c_str(), &m_isEnabled)) {
		wasModified = true;
	}

	ImGui::Text("\tpower %f,%f,%f", m_power[0], m_power[1], m_power[2]);
	ImGui::Text("\tposition %f,%f,%f", m_position[0], m_position[1], m_position[2]);

	return wasModified;
}