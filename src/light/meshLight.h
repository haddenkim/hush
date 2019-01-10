#pragma once

#include "light/light.h"
class Mesh;

class MeshLight : public Light {
public:
	MeshLight(Mesh* mesh);

	LightSample sampleLi(const Point3f surfacePosition, Sampler& sampler) const override;
	Spectrum le() const override;

	Mesh* m_mesh;
	Spectrum m_radiance; // aka Lemit
};