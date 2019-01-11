#include "material/material.h"
#include "texture/texture.h"

void Material::guiRead()
{

	ImGui::Text("name: %s", m_name.c_str());

	if (m_diffuseTexture) {
		ImGui::Text("diffuse textured %i,%i", m_diffuseTexture->m_width, m_diffuseTexture->m_height);
	} else {
		ImGui::Text("diffuse %f,%f,%f", m_diffuse[0], m_diffuse[1], m_diffuse[2]);
	}

	ImGui::Text("ambient %f,%f,%f", m_ambient[0], m_ambient[1], m_ambient[2]);
	ImGui::Text("specular %f,%f,%f", m_specular[0], m_specular[1], m_specular[2]);
	ImGui::Text("transmittance %f,%f,%f", m_transmittance[0], m_transmittance[1], m_transmittance[2]);
	ImGui::Text("emission %f,%f,%f", m_emission[0], m_emission[1], m_emission[2]);
	ImGui::Text("shininess %f", m_shininess);
	ImGui::Text("refractionIndex %f", m_refractionIndex);
}