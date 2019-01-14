#include "material/lambertianMaterial.h"
#include "material/phongMaterial.h"
#include "texture/texture.h"

void LambertianMaterial::guiRead() const
{
	ImGui::Text("type: Lambertian");
	ImGui::Text("name: %s", m_name.c_str());

	ImGui::Text("ambient %f,%f,%f", m_ambient[0], m_ambient[1], m_ambient[2]);
	if (m_ambientMap)
		ImGui::Text("ambient textured %i,%i", m_ambientMap->m_width, m_ambientMap->m_height);

	ImGui::Text("diffuse %f,%f,%f", m_diffuse[0], m_diffuse[1], m_diffuse[2]);
	if (m_diffuseMap)
		ImGui::Text("diffuse textured %i,%i", m_diffuseMap->m_width, m_diffuseMap->m_height);

	ImGui::Text("emission %f,%f,%f", m_emission[0], m_emission[1], m_emission[2]);
}

void PhongMaterial::guiRead() const
{
	ImGui::Text("type: Phong");
	ImGui::Text("name: %s", m_name.c_str());

	ImGui::Text("ambient %f,%f,%f", m_ambient[0], m_ambient[1], m_ambient[2]);
	if (m_ambientMap)
		ImGui::Text("ambient textured %i,%i", m_ambientMap->m_width, m_ambientMap->m_height);

	ImGui::Text("diffuse %f,%f,%f", m_diffuse[0], m_diffuse[1], m_diffuse[2]);
	if (m_diffuseMap)
		ImGui::Text("diffuse textured %i,%i", m_diffuseMap->m_width, m_diffuseMap->m_height);

	ImGui::Text("specular %f,%f,%f", m_specular[0], m_specular[1], m_specular[2]);
	if (m_specularMap)
		ImGui::Text("specular textured %i,%i", m_specularMap->m_width, m_specularMap->m_height);

	ImGui::Text("shininess %f", m_shininess);
	if (m_shininessMap)
		ImGui::Text("shininess textured %i,%i", m_shininessMap->m_width, m_shininessMap->m_height);

	ImGui::Text("emission %f,%f,%f", m_emission[0], m_emission[1], m_emission[2]);
}