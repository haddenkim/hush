#pragma once

#include "common.h"

struct Texture{
	Spectrum sample(const Point2f& texcoords) const;

	unsigned char* m_data; // 3 channel - rgb, row major, starts bottom-left
	uint m_width;
	uint m_height;
};