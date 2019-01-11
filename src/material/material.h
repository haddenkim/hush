#pragma once

#include "common.h"
#include <string>
#include "gui/iGuiReadable.h"

class Sampler;
struct MaterialSample;
struct Texture;
struct SurfaceInteraction;

struct Material: public IGuiReadable {

	Spectrum evaluate(const SurfaceInteraction& surfaceInteraction) const;
	float pdf(const SurfaceInteraction& surfaceInteraction) const;
	MaterialSample sample_f(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const;

	Spectrum getDiffuse(const Point2f& texcoord) const;

	// UI
	void guiRead() const override;

	// data
	std::string m_name;

	bool m_isDiffuse;
	bool m_isSpecular;
	bool m_isTransmissive;
	bool m_isEmissive;

	Spectrum m_ambient;		  // Ka
	Spectrum m_diffuse;		  // Kd
	Spectrum m_specular;	  // Ks
	Spectrum m_transmittance; // Kt
	Spectrum m_emission;	  // Ke
	float m_shininess;		  // alpha
	float m_refractionIndex;  // nu

	// CODEHERE - additional textures for specular, transmissive, ambient, etc 
	Texture* m_diffuseTexture;
	

	// CODEHERE - load additional obj-mtl properties
	// real_t dissolve;			// 1 == opaque; 0 == fully transparent
	// int illum;					// illumination model (see http://www.fileformat.info/format/material/)
	// real_t roughness;			// [0, 1] default 0
	// real_t metallic;			// [0, 1] default 0
	// real_t sheen;				// [0, 1] default 0
	// real_t clearcoat_thickness; // [0, 1] default 0
	// real_t clearcoat_roughness; // [0, 1] default 0
	// real_t anisotropy;			// aniso. [0, 1] default 0
	// real_t anisotropy_rotation; // anisor. [0, 1] default 0
};