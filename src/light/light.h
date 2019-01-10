#pragma once

#include "common.h"
#include <string>

class LightSample;
class Sampler;

class Light {
public:
	Light(const std::string name, const Vec3f position, const Spectrum power);

	virtual LightSample sampleLi(const Point3f surfacePosition, Sampler& sampler) const = 0; // incident radiance
	virtual Spectrum le() const;															 // emmited radiance
	// CODEHERE - refactor evaluate and pdf functions from PtRenderer to here 
	// virtual float pdf()
	// virtual Spectrum evaluate()

	bool m_isDelta;

	// for GL and GUI
	const std::string m_name;
	const Vec3f m_position;
	const Spectrum m_power;
	bool m_isEnabled = true;
};