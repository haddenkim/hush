#include "vec3fBuffer.h"

Vec3fBuffer::Vec3fBuffer(PipelineIO type, uint width, uint height)
	: CpuBuffer(type, width, height)
{
	m_data = std::vector<Vec3f>(width * height, Vec3f(0.f));
}

void Vec3fBuffer::passToGPU(GLuint texId)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, &m_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Vec3fBuffer::clear()
{
	std::fill(m_data.begin(), m_data.end(), Vec3f(0.f));
}
