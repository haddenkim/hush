#pragma once
#include "common.h"
#include "pipeline/pipelineIO.h"
#include <glad/glad.h>

// CODEHERE enum buffer type (ex. spectrum, signed unit vector, world position)

class Buffer {
public:
	// Flags
	const PipelineIO m_type;
	const PipelineHW m_hardware;

protected:
	Buffer(PipelineIO type, uint width, uint height, PipelineHW hw);

	const uint m_width;
	const uint m_height;
};