#pragma once
#include "common.h"
#include "pipeline/pipelineIO.h"
#include <glad/glad.h>

// CODEHERE enum buffer type (ex. spectrum, signed unit vector, world position)

class Buffer {
public:
	Buffer(PipelineIO type, uint width, uint height);

	void prepareToDisplay();

	const GLuint m_glId;
	const PipelineIO m_type;

protected:
	static GLuint setupGL(PipelineIO type, uint width, uint height);

	const uint m_width;
	const uint m_height;

	bool m_isReadyToDisplay;
};