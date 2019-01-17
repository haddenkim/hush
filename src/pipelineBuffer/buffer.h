#pragma once
#include "common.h"
#include "pipeline/pipelineIO.h"
#include <glad/glad.h>

// CODEHERE enum buffer type (ex. spectrum, signed unit vector, world position)


class Buffer {
public:
	static Buffer* create(PipelineIO type, uint width, uint height, bool isCPU = false);

	virtual void prepareToDisplay();

	const GLuint m_glId;
	const PipelineIO m_type;
	bool m_isReadyToDisplay;

protected:
	Buffer(PipelineIO type, uint width, uint height, bool isCPU = false);

	static GLuint setupGL(PipelineIO type, uint width, uint height);

	const uint m_width;
	const uint m_height;
};