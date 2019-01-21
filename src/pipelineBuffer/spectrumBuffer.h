#include "common.h"
#include "pipelineBuffer/cpuBuffer.h"
#include <glad/glad.h>
#include <vector>

class SpectrumBuffer : public CpuBuffer {
public:
	SpectrumBuffer(PipelineIO type, uint width, uint height);

	void passToGPU(GLuint texId) override;

	void clear() override;

	// data
	std::vector<Spectrum> m_data;
};