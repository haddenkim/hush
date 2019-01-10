#pragma once

#include "common.h"

struct MaterialSample {
	Spectrum m_albedo;
	Vec3f m_direction;
	float m_pdf;
};
