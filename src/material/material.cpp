#include "material.h"
#include "interaction/surfaceInteraction.h"
#include "material/materialSample.h"
#include "math/vector.h"
#include "sampler/sampling.h"
#include "texture/texture.h"

Spectrum Material::getDiffuse(const Point2f& texcoord) const
{
	if (m_diffuseTexture != nullptr) {
		return m_diffuseTexture->sample(texcoord);
	} else {
		return m_diffuse;
	}
}

Spectrum Material::evaluate(const SurfaceInteraction& surfaceInteraction) const
{
	// CODEHERE - use more complex/hybrid bsdf model to include specular, transmission, etc.

	// lambertian reflection
	return surfaceInteraction.m_diffuse * M_1_PI;
	// return getDiffuse(surfaceInteraction.m_texCoord) * M_1_PI;
}

float Material::pdf(const SurfaceInteraction& surfaceInteraction) const
{
	// CODEHERE - use more complex/hybrid bsdf model to include specular, transmission, etc.

	// lambertian reflection
	return glm::dot(surfaceInteraction.m_normalShade, surfaceInteraction.m_wi) * M_1_PI;
}

MaterialSample Material::sample_f(SurfaceInteraction& surfaceInteraction, Sampler& sampler) const
{
	// CODEHERE - use more complex/hybrid bsdf model to include specular, transmission, etc.

	// lambertian reflection
	MaterialSample ret;

	// sample direction in local (surface) space
	Vec3f sampleDirection = CosineSampleHemisphere(sampler);

	// transform to world space
	ret.m_direction = surfaceInteraction.m_wi = local2World(sampleDirection, surfaceInteraction.m_normalShade);

	ret.m_albedo = evaluate(surfaceInteraction);
	ret.m_pdf = pdf(surfaceInteraction);
	return ret;
}
