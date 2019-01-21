#include "gpuBuffer.h"

GpuBuffer::GpuBuffer(PipelineIO type, uint width, uint height)
	: Buffer(type, width, height, GPU)
	, m_texId(setupTexture(type, width, height))
{
}

GLuint GpuBuffer::setupTexture(PipelineIO type, uint width, uint height)
{
	/* determine openGL storage format */
	GLenum format;

	switch (type) {
	case G_POSITION:
	case G_NORMAL:
	case G_MAT_AMBIENT:
	case G_MAT_DIFFUSE:
	case LIGHT_DIRECT:
	case LIGHT_INDIRECT:
	case COLOR:
		format = GL_RGB;
		break;

	case G_MAT_SPECULAR:
	case G_MAT_TRANSMISSIVE:
		format = GL_RGBA;
		break;

	default:
		assert(!"The default case of switch was reached.");
		break;
	}

	// create opengl texture
	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}
