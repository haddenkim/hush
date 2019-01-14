#pragma once
#include "common.h"
#include "material/material.h"

class LambertianMaterial : public Material {
public:
	LambertianMaterial(std::string name,
					   Spectrum diffuse,
					   Texture* diffuseMap = nullptr);

	Spectrum evaluate(const SurfaceInteraction& surfaceInteraction) const override;
	float pdf(const SurfaceInteraction& surfaceInteraction) const override;
	MaterialSample sample(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const override;

	Spectrum getDiffuse(const Point2f& texcoord) const override;

	// for OpenGL
	GlMaterial getGL() const override;

	// UI
	void guiRead() const override;

	// data
	Spectrum m_diffuse; // Kd

	Texture* m_diffuseMap; // map_Kd
};