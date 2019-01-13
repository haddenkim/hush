#pragma once

#include "common.h"
#include "gui/iGuiReadable.h"
#include <string>

class Sampler;
struct MaterialSample;
struct Texture;
struct SurfaceInteraction;

struct Material : public IGuiReadable {

	Spectrum evaluate(const SurfaceInteraction& surfaceInteraction) const;
	float pdf(const SurfaceInteraction& surfaceInteraction) const;
	MaterialSample sample_f(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const;

	// get from possibly textured values
	Spectrum getAmbient(const Point2f& texcoord) const;
	Spectrum getDiffuse(const Point2f& texcoord) const;
	Spectrum getSpecular(const Point2f& texcoord) const;
	float getShininess(const Point2f& texcoord) const;

	// UI
	void guiRead() const override;

	// data
	std::string m_name;
	Spectrum m_ambient;		  // Ka
	Spectrum m_diffuse;		  // Kd
	Spectrum m_specular;	  // Ks
	float m_shininess;		  // Ns aka specular hightlight
	Spectrum m_transmittance; // Kt aka Tf
	float m_refractionIndex;  // Ni aka greek nu aka ior
	Spectrum m_emission;	  // Ke

	Texture* m_ambientMap;   // map_Ka
	Texture* m_diffuseMap;   // map_Kd
	Texture* m_specularMap;  // map_Ks
	Texture* m_shininessMap; // map_Ns

	bool m_isDiffuse;
	bool m_isSpecular;
	bool m_isTransmissive;
	bool m_isEmissive;

	// CODEHERE - load additional obj-mtl properties and textures
	// real_t dissolve;			// 1 == opaque; 0 == fully transparent
	// int illum;					// illumination model (see http://www.fileformat.info/format/material/)
	// real_t roughness;			// [0, 1] default 0
	// real_t metallic;			// [0, 1] default 0
	// real_t sheen;				// [0, 1] default 0
	// real_t clearcoat_thickness; // [0, 1] default 0
	// real_t clearcoat_roughness; // [0, 1] default 0
	// real_t anisotropy;			// aniso. [0, 1] default 0
	// real_t anisotropy_rotation; // anisor. [0, 1] default 0
	// bump_texname;				// map_bump, map_Bump, bump
	// displacement_texname;		// disp
	// alpha_texname;				// map_d
	// reflection_texname;			// refl
};