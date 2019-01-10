#pragma once

#include "common.h"
#include "sampler/sampler.h"

/* cosine hemisphere */
inline Vec3f CosineSampleHemisphere(Sampler& sampler)
{
	Vec2f sample = sampler.get2D();
	float phi = sample.x * float(2.0 * M_PI);
	float cosTheta = std::sqrt(sample.y);
	float sinTheta = std::sqrt(1.f - sample.y);

	return Vec3f(sinTheta * std::cos(phi),
				 sinTheta * std::sin(phi),
				 cosTheta);
}

inline float CosineHemispherePdf(const Vec3f& v)
{
	return std::abs(v.z) * float(M_1_PI);
}

/* power cosine hemisphere */
inline Vec3f powerCosineHemisphere(const Point2f& sample, const float n)
{
	float phi = sample.x * float(2.0 * M_PI);
	float cosTheta = std::pow(sample.y, 1.f / (n + 1.f));
	float sinTheta = std::sqrt(std::max(0.f, 1.f - (cosTheta * cosTheta)));

	return Vec3f(sinTheta * std::cos(phi),
				 sinTheta * std::sin(phi),
				 cosTheta);
}

inline float powerCosineHemispherePDF(const Vec3f& v, const float n)
{
	return (n + 1.0f) * float(M_2_PI) * std::pow(v.z, n);
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
	float f = nf * fPdf;
	float g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}