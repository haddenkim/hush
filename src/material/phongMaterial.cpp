#include "phongMaterial.h"
#include "interaction/surfaceInteraction.h"
#include "material/materialSample.h"
#include "math/vector.h"
#include "sampler/sampling.h"
#include "texture/texture.h"

PhongMaterial::PhongMaterial(std::string name,
							 Spectrum diffuse,
							 Spectrum specular,
							 float shininess,
							 Texture* diffuseMap,
							 Texture* specularMap,
							 Texture* shininessMap)
	: m_diffuse(diffuse)
	, m_specular(specular)
	, m_shininess(shininess)
	, m_diffuseMap(diffuseMap)
	, m_specularMap(specularMap)
	, m_shininessMap(shininessMap)
{
	m_name = name;
	m_isDiffuse = true;
	m_isSpecular = true;
	m_isTransmissive = false;
	m_isEmissive = false;
}

Spectrum PhongMaterial::evaluate(const SurfaceInteraction& surfaceInteraction) const
{
	Spectrum R(0.f); // reflectance

	if (m_isDiffuse) {
		R += surfaceInteraction.m_diffuse * ONE_OVER_PI;
	}

	if (m_isSpecular) {
		Spectrum Ks = surfaceInteraction.m_specular;
		float Ns = surfaceInteraction.m_shininess;
		Vec3f reflectDir = glm::reflect(-surfaceInteraction.m_wo, surfaceInteraction.m_normalShade);
		float cosAngle = glm::clamp(glm::dot(reflectDir, surfaceInteraction.m_wi), 0.f, 1.f);
		R += Ks * (Ns + 2.f) * ONE_OVER_2PI * pow(cosAngle, Ns);
	}
	return R;
}

float PhongMaterial::pdf(const SurfaceInteraction& surfaceInteraction) const
{
	float pdfD;
	float pdfS;
	if (m_isDiffuse) {
		pdfD = glm::dot(surfaceInteraction.m_normalShade, surfaceInteraction.m_wi) * ONE_OVER_PI;
	}

	if (m_isSpecular) {
		float Ns = surfaceInteraction.m_shininess;
		Vec3f reflectDir = glm::reflect(-surfaceInteraction.m_wo, surfaceInteraction.m_normalShade);
		float cosAngle = glm::clamp(glm::dot(reflectDir, surfaceInteraction.m_wi), 0.f, 1.f);

		pdfS = (Ns + 1.f) * ONE_OVER_2PI * pow(cosAngle, Ns);
	}

	float maxKD = MaxComponent(m_diffuse);
	float maxKS = MaxComponent(m_specular);
	float diffuseRatio = maxKD / (maxKD + maxKS);

	return diffuseRatio * pdfD + (1.f - diffuseRatio) * pdfS;
}

MaterialSample PhongMaterial::sample(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const
{
	MaterialSample ret;

	// choose a component
	float maxKD = MaxComponent(m_diffuse);
	float maxKS = MaxComponent(m_specular);
	float u = sampler.get1D();

	Vec3f sampleDirection(0.f); // sample direction in local (surface) space
	if (u < maxKD) {
		// diffuse
		sampleDirection = CosineSampleHemisphere(sampler);
		ret.m_direction = surfaceInteraction.m_wi = local2World(sampleDirection, surfaceInteraction.m_normalShade);

	} else if (u < maxKD + maxKS) {
		// specular
		sampleDirection = powerCosineHemisphere(sampler, surfaceInteraction.m_shininess);
		Vec3f reflectDir = glm::reflect(-surfaceInteraction.m_wo, surfaceInteraction.m_normalShade);
		ret.m_direction = surfaceInteraction.m_wi = local2World(sampleDirection, reflectDir);

		if (sampleDirection.z < 0.f) {
			return ret;
		}

	} else {
		// ray is absorbed, choose neither
		return ret;
	}

	// transform to world space
	ret.m_albedo = evaluate(surfaceInteraction);
	ret.m_pdf = pdf(surfaceInteraction);
	return ret;
}

Spectrum PhongMaterial::getDiffuse(const Point2f& texcoord) const
{
	if (m_diffuseMap != nullptr) {
		return m_diffuseMap->sample(texcoord) * m_diffuse;
	} else {
		return m_diffuse;
	}
}

Spectrum PhongMaterial::getSpecular(const Point2f& texcoord) const
{
	if (m_specularMap != nullptr) {
		return m_specularMap->sample(texcoord) * m_specular;
	} else {
		return m_specular;
	}
}

float PhongMaterial::getShininess(const Point2f& texcoord) const
{
	if (m_specularMap != nullptr) {
		return m_shininessMap->sample(texcoord).r * m_shininess;
	} else {
		return m_shininess;
	}
}

GlMaterial PhongMaterial::getGL() const
{
	GlMaterial ret;
	ret.m_ambient = m_ambient;
	ret.m_diffuse = m_diffuse;
	ret.m_specular = m_specular;
	ret.m_shininess = m_shininess;
	ret.m_emission = m_shininess;

	ret.m_ambientMap = m_ambientMap;
	ret.m_diffuseMap = m_diffuseMap;
	ret.m_specularMap = m_specularMap;
	ret.m_shininessMap = m_shininessMap;

	return ret;
}
