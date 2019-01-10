#pragma once

#include "common.h"

struct LightSample
{
	Spectrum m_radiance;
	Vec3f m_direction;
	float m_distance;
	float m_pdf;
};