#pragma once

#include "common.h"
#include "gui/iGuiReadable.h"
#include <string>

class Sampler;
struct MaterialSample;
struct Texture;
struct SurfaceInteraction;

struct GlMaterial {
	Spectrum m_ambient;  // Ka
	Spectrum m_diffuse;  // Kd
	Spectrum m_specular; // Ks
	float m_shininess;   // Ns aka specular hightlight
	Spectrum m_emission; // Ke

	Texture* m_ambientMap;   // map_Ka
	Texture* m_diffuseMap;   // map_Kd
	Texture* m_specularMap;  // map_Ks
	Texture* m_shininessMap; // map_Ns
};

struct Material : public IGuiReadable {

	virtual Spectrum evaluate(const SurfaceInteraction& surfaceInteraction) const = 0;
	virtual float pdf(const SurfaceInteraction& surfaceInteraction) const = 0;
	virtual MaterialSample sample(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const = 0;

	// get from possibly textured values
	virtual Spectrum getAmbient(const Point2f& texcoord) const;
	virtual Spectrum getDiffuse(const Point2f& texcoord) const;
	virtual Spectrum getSpecular(const Point2f& texcoord) const;
	virtual float getShininess(const Point2f& texcoord) const;

	// for OpenGL
	virtual GlMaterial getGL() const;

	// UI
	virtual void guiRead() const override = 0;

	// data
	std::string m_name;

	Spectrum m_ambient; // Ka
	Spectrum m_emission; // Ke

	Texture* m_ambientMap; // map_Ka

	bool m_isDiffuse;
	bool m_isSpecular;
	bool m_isTransmissive;
	bool m_isEmissive;

};