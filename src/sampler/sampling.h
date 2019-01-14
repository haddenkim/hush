#pragma once

#include "common.h"
#include "sampler/sampler.h"

/* cosine hemisphere */
inline Vec3f CosineSampleHemisphere(Sampler& sampler)
{
	Vec2f sample = sampler.get2D();
	float phi = sample.x * TWO_PI;
	float cosTheta = std::sqrt(sample.y);
	float sinTheta = std::sqrt(1.f - sample.y);

	return Vec3f(sinTheta * std::cos(phi),
				 sinTheta * std::sin(phi),
				 cosTheta);
}


/* power cosine hemisphere */
inline Vec3f powerCosineHemisphere(Sampler& sampler, const float n)
{
	Vec2f sample = sampler.get2D();
	float phi = sample.x * TWO_PI;
	float cosTheta = std::pow(sample.y, 1.f / (n + 1.f));
	float sinTheta = std::sqrt(std::max(0.f, 1.f - (cosTheta * cosTheta)));

	return Vec3f(sinTheta * std::cos(phi),
				 sinTheta * std::sin(phi),
				 cosTheta);
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
	float f = nf * fPdf;
	float g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}