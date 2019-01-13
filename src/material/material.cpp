#include "material.h"
#include "interaction/surfaceInteraction.h"
#include "material/materialSample.h"
#include "math/vector.h"
#include "sampler/sampling.h"
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
	if (m_diffuseMap != nullptr) {
		return m_diffuseMap->sample(texcoord) * m_diffuse;
	} else {
		return m_diffuse;
	}
}

Spectrum Material::getSpecular(const Point2f& texcoord) const
{
	if (m_specularMap != nullptr) {
		return m_specularMap->sample(texcoord) * m_specular;
	} else {
		return m_specular;
	}
}

float Material::getShininess(const Point2f& texcoord) const
{
	if (m_specularMap != nullptr) {
		return m_shininessMap->sample(texcoord).r * m_shininess;
	} else {
		return m_shininess;
	}
}

Spectrum Material::evaluate(const SurfaceInteraction& surfaceInteraction) const
{
		// CODEHERE - use more complex/hybrid bsdf model to include transmission.
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

float Material::pdf(const SurfaceInteraction& surfaceInteraction) const
{
	// CODEHERE - use more complex/hybrid bsdf model to include transmission.
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

MaterialSample Material::sample_f(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const
{
	// CODEHERE - use more complex/hybrid bsdf model to include transmission.
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
