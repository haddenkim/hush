#pragma once
#include "common.h"
#include "material/material.h"

class PhongMaterial : public Material {
public:
	PhongMaterial(std::string name,
				  Spectrum diffuse,
				  Spectrum specular,
				  float shininess,
				  Texture* diffuseMap = nullptr,
				  Texture* specularMap = nullptr,
				  Texture* shininessMap = nullptr
	);

	Spectrum evaluate(const SurfaceInteraction& surfaceInteraction) const override;
	float pdf(const SurfaceInteraction& surfaceInteraction) const override;
	MaterialSample sample(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const override;

	Spectrum getDiffuse(const Point2f& texcoord) const override;
	Spectrum getSpecular(const Point2f& texcoord) const override;
	float getShininess(const Point2f& texcoord) const override;

	// for OpenGL
	GlMaterial getGL() const override;

	// UI
	void guiRead() const override;

	// data
	Spectrum m_diffuse;  // Kd
	Spectrum m_specular; // Ks
	float m_shininess;   // Ns aka specular hightlight

	Texture* m_diffuseMap;   // map_Kd
	Texture* m_specularMap;  // map_Ks
	Texture* m_shininessMap; // map_Ns
};