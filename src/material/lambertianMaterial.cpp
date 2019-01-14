#include "lambertianMaterial.h"
#include "interaction/surfaceInteraction.h"
#include "material/materialSample.h"
#include "math/vector.h"
#include "sampler/sampling.h"
#include "texture/texture.h"

LambertianMaterial::LambertianMaterial(std::string name, Spectrum diffuse, Texture* diffuseMap)
	: m_diffuse(diffuse)
	, m_diffuseMap(diffuseMap)
{
	m_name = name;
	m_isDiffuse = true;
	m_isSpecular = false;
	m_isTransmissive = false;
	m_isEmissive = false;
}

Spectrum LambertianMaterial::evaluate(const SurfaceInteraction& surfaceInteraction) const
{
	return surfaceInteraction.m_diffuse * ONE_OVER_PI;
}

float LambertianMaterial::pdf(const SurfaceInteraction& surfaceInteraction) const
{
	return glm::dot(surfaceInteraction.m_normalShade, surfaceInteraction.m_wi) * ONE_OVER_PI;
}

MaterialSample LambertianMaterial::sample(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const
{
	MaterialSample ret;

	Vec3f sampleDirection = CosineSampleHemisphere(sampler);
	ret.m_direction = surfaceInteraction.m_wi = local2World(sampleDirection, surfaceInteraction.m_normalShade);

	ret.m_albedo = evaluate(surfaceInteraction);
	ret.m_pdf = pdf(surfaceInteraction);
	return ret;
}

Spectrum LambertianMaterial::getDiffuse(const Point2f& texcoord) const
{
	if (m_diffuseMap != nullptr) {
		return m_diffuseMap->sample(texcoord) * m_diffuse;
	} else {
		return m_diffuse;
	}
}

GlMaterial LambertianMaterial::getGL() const
{
	GlMaterial ret;
	ret.m_ambient = m_ambient;
	ret.m_diffuse = m_diffuse;

	ret.m_ambientMap = m_ambientMap;
	ret.m_diffuseMap = m_diffuseMap;

	return ret;
}
