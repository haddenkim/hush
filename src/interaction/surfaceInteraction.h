#pragma once

#include "common.h"

class Material;
class Light;
class RTCRayHit;
class Scene;
class Mesh;

struct SurfaceInteraction {

	SurfaceInteraction(const RTCRayHit& rayHit, const Scene& scene);
	SurfaceInteraction(const RTCRayHit& rayHit, const Mesh& mesh);

	Vec3f m_position;	// P
	Vec3f m_wo;			 // direction out, world space
	Vec3f m_wi;			 // direction in, world space
	Vec3f m_normalGeo;   // N_g
	Vec3f m_normalShade; // N_s
	Vec2f m_texCoord;	// uv

	Spectrum m_diffuse; 
	Spectrum m_specular;
	float m_shininess;

	Material* m_material;
	Light* m_light;
};