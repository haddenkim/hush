#include "material/material.h"
#include "scene/mesh.h"

void Mesh::guiRead()
{
	if (ImGui::TreeNode(m_name.c_str())) {
		ImGui::Text("Center (%f,%f,%f)", m_center.x, m_center.y, m_center.z);
		ImGui::Text("Min Bounds (%f,%f,%f)", m_minBounds.x, m_minBounds.y, m_minBounds.z);
		ImGui::Text("Max Bounds (%f,%f,%f)", m_maxBounds.x, m_maxBounds.y, m_maxBounds.z);
		ImGui::Separator();

		ImGui::Text("Vertices %lu", m_vertices.size());
		ImGui::Text("Triangles %lu", m_faces.size());
		ImGui::Separator();

		if (ImGui::TreeNode("Material")) {
			Material* material = m_material;
			material->guiRead();

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

bool Mesh::guiEdit()
{
	bool wasModified = false;

	if (ImGui::Checkbox(m_name.c_str(), &m_isEnabled)) {
		wasModified = true;
	}

	return wasModified;
}