#include "material.h"
#include "texture/texture.h"

Spectrum Material::getAmbient(const Point2f& texcoord) const
{
	if (m_ambientMap != nullptr) {
		return m_ambientMap->sample(texcoord) * m_ambient;
	} else {
		return m_ambient;
	}
}

Spectrum Material::getDiffuse(const Point2f& texcoord) const
{
	return Spectrum(0.f);
}

Spectrum Material::getSpecular(const Point2f& texcoord) const
{
	return Spectrum(0.f);
}

float Material::getShininess(const Point2f& texcoord) const
{
	return 0.f;
}

GlMaterial Material::getGL() const
{
	return GlMaterial();
}
