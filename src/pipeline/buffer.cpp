#include "buffer.h"
#include <cassert>

Buffer::Buffer(PipelineIO type, uint width, uint height)
	: m_type(type)
	, m_width(width)
	, m_height(height)
	, m_glId(setupGL(type, width, height))
{
}

void Buffer::prepareToDisplay()
{
	if (!m_isReadyToDisplay) {

		m_isReadyToDisplay = true;
	}
}

GLuint Buffer::setupGL(PipelineIO type, uint width, uint height)
{
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