#pragma once
#include "common.h"
#include "pipeline/pipelineIO.h"
#include <glad/glad.h>
#include <vector>

class Buffer;
class GpuBuffer;
class SpectrumBuffer;
;

class BufferManager {
public:
	BufferManager(uint width, uint height);

	Buffer* requestBuffer(PipelineIO type, PipelineHW hw);
	GpuBuffer* requestGpuBuffer(PipelineIO type);
	SpectrumBuffer* requestSpectrumBuffer(PipelineIO type);

	GLuint getGlTextureId(size_t index);

	size_t size() const;
	Buffer* at(size_t index) const;

protected:
	Buffer* create(PipelineIO type, PipelineHW hw);

	const uint m_width;
	const uint m_height;
	std::vector<Buffer*> m_buffers;

	GLuint m_cpuBufferTexId;
};