#include "light.h"

Light::Light(const std::string name, const Vec3f position, const Spectrum power)
	: m_name(name)
	, m_position(position)
	, m_power(power)
{
}

Spectrum Light::le() const
{
	return Spectrum(0.f);
}