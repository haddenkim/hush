#include "vec3fBuffer.h"

Vec3fBuffer::Vec3fBuffer(PipelineIO type, uint width, uint height)
	: Buffer(type, width, height, true)
{
	m_data = std::vector<Vec3f>(width * height, Spectrum(0.f));
}

void Vec3fBuffer::prepareToDisplay()
{
	if (!m_isReadyToDisplay) {
		// send data to gpu
		glBindTexture(GL_TEXTURE_2D, m_glId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, &m_data[0]);

		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);

		m_isReadyToDisplay = true;
	}
}