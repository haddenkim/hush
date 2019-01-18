#pragma once

#include "common.h"
#include "pipelineBuffer/buffer.h"
#include <glad/glad.h>

class CpuBuffer : public Buffer {
public:
	virtual void passToGPU(GLuint texId) = 0;

protected:
	CpuBuffer(PipelineIO type, uint width, uint height);
};