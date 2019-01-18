#include "common.h"
#include "pipelineBuffer/cpuBuffer.h"
#include <vector>

class Vec3fBuffer : public CpuBuffer {
public:
	Vec3fBuffer(PipelineIO type, uint width, uint height);

	void passToGPU(GLuint texId) override;

	std::vector<Vec3f> m_data;
};