#include "bufferManager.h"

#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/cpuBuffer.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "pipelineBuffer/spectrumBuffer.h"

BufferManager::BufferManager(uint width, uint height)
	: m_width(width)
	, m_height(height)
{
	glGenTextures(1, &m_cpuBufferTexId);
}

Buffer* BufferManager::requestBuffer(PipelineIO type, PipelineHW hw)
{
	// search for existing buffer matching type and hw
	for (Buffer* buffer : m_buffers) {
		if (buffer->m_type == type && buffer->m_hardware == hw) {
			return buffer;
		}
	}

	// matching buffer not found. create and return new buffer
	return create(type, hw);
}

GpuBuffer* BufferManager::requestGpuBuffer(PipelineIO type)
{
	Buffer* buffer = requestBuffer(type, GPU);
	return static_cast<GpuBuffer*>(buffer);
}

SpectrumBuffer* BufferManager::requestSpectrumBuffer(PipelineIO type)
{
	Buffer* buffer = requestBuffer(type, CPU);
	return static_cast<SpectrumBuffer*>(buffer);
}

Buffer* BufferManager::create(PipelineIO type, PipelineHW hw)
{
	Buffer* buffer;

	if (hw == GPU) {
		buffer = new GpuBuffer(type, m_width, m_height);
	}
	// CPU buffers
	else if (type == RT_COLOR) {
		buffer = new SpectrumBuffer(type, m_width, m_height);
	}

	m_buffers.push_back(buffer);
	return buffer;
}

GLuint BufferManager::getGlTextureId(size_t index)
{
	assert(index < m_buffers.size());

	Buffer* buffer = m_buffers[index];
	if (buffer->m_hardware == GPU) {
		return static_cast<GpuBuffer*>(buffer)->m_texId;
	}

	// CPU buffers
	else {
		static_cast<CpuBuffer*>(buffer)->passToGPU(m_cpuBufferTexId);
		return m_cpuBufferTexId;
	}
}

size_t BufferManager::size() const
{
	return m_buffers.size();
}

Buffer* BufferManager::at(size_t index) const
{
	return m_buffers.at(index);
}
