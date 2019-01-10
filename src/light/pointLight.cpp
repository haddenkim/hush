#include "pointLight.h"
#include "light/lightSample.h"

PointLight::PointLight(const std::string name, Point3f position, Spectrum intensity)
	: Light(name, position, intensity * 4 * M_PI)
	, m_intensity(intensity)
{
	m_isDelta = true;
}

LightSample PointLight::sampleLi(const Point3f surfacePosition, Sampler& sampler) const
{
	LightSample ret;

	ret.m_direction = glm::normalize(m_position - surfacePosition);
	ret.m_distance = glm::distance(m_position, surfacePosition);
	ret.m_pdf = 1.f;
	// CODEHERE - optimize distance squared is cheaper, so calculate it first then sqrt for distance
	ret.m_radiance = m_intensity / (ret.m_distance * ret.m_distance);

	return ret;
}
