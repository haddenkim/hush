#include "bufferSync.h"
#include "common.h"
#include <cassert>
#include <glad/glad.h>

#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/bufferSync.h"
#include "pipelineBuffer/cpuBuffer.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "pipelineBuffer/spectrumBuffer.h"
#include "pipelineBuffer/vec3fBuffer.h"

BufferSync::BufferSync(Buffer* fromBuffer, Buffer* toBuffer)
	: m_fromBuffer(fromBuffer)
	, m_toBuffer(toBuffer)
	, m_toHardware(toBuffer->m_hardware)
{
}

void BufferSync::sync()
{
	if (m_toHardware == GPU) {
		GLuint texId = static_cast<GpuBuffer*>(m_toBuffer)->m_texId;
		static_cast<CpuBuffer*>(m_fromBuffer)->passToGPU(texId);
	}

	else {
		assert(!"not yet implemented");
	}
}