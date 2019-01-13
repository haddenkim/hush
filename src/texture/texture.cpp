#include "texture.h"
#include <cmath>
// #include <glm/glm.hpp>

Spectrum Texture::sample(const Point2f& texcoords) const
{
	// CODEHERE - GUI offer alternative texture wrapping modes (repeat, mirror, black, etc)

	// CLAMP_TO_EDGE strategy
	// Point2f unwrappedTexcoords = glm::clamp(texcoords, Point2f(0.f), Point2f(1.f));

	// REPEAT strategy
	Point2f unwrappedTexcoords = glm::mod(texcoords, 1.f);

	uint dataX = unwrappedTexcoords.x * (float)(m_width - 1);
	uint dataY = unwrappedTexcoords.y * (float)(m_height - 1);
	uint index = ((dataY * m_width) + dataX) * m_numChannels;

	// 1 channel (bump maps)
	if (m_numChannels == 1) {
		unsigned char r = m_data[index];
		float rf = (float)r / 255.f;
		return Spectrum(rf);

		// 3 (RGB) or 4 (RGBA) channel
	} else if (m_numChannels == 3 || m_numChannels == 4) {
		unsigned char r = m_data[index];
		unsigned char g = m_data[index + 1];
		unsigned char b = m_data[index + 2];

		return Spectrum((float)r / 255.f,
						(float)g / 255.f,
						(float)b / 255.f);
	} else {
		assert(!"Unsupported number of Texture channels");
		return Spectrum(0.f);
	}
}
