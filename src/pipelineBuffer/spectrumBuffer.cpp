#include "spectrumBuffer.h"

SpectrumBuffer::SpectrumBuffer(PipelineIO type, uint width, uint height)
	: CpuBuffer(type, width, height)
{
	m_data = std::vector<Spectrum>(width * height, Spectrum(0.f));
}

void SpectrumBuffer::passToGPU(GLuint texId)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, &m_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}
