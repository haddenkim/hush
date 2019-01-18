#include "common.h"
#include "pipelineBuffer/buffer.h"
#include <glad/glad.h>

class GpuBuffer : public Buffer {
public:
	GpuBuffer(PipelineIO type, uint width, uint height);

	const GLuint m_texId;

protected:
	// setup helper
	static GLuint setupTexture(PipelineIO type, uint width, uint height);
};