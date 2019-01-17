#include "spectrumBuffer.h"

SpectrumBuffer::SpectrumBuffer(PipelineIO type, uint width, uint height)
	: Buffer(type, width, height, true)
{
	m_data = std::vector<Spectrum>(width * height, Spectrum(0.f));
}

void SpectrumBuffer::prepareToDisplay()
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