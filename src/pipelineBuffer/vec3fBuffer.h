#include "common.h"
#include "pipelineBuffer/buffer.h"
#include <vector>

class Vec3fBuffer : public Buffer {
public:
	Vec3fBuffer(PipelineIO type, uint width, uint height);

	void prepareToDisplay() override;

	std::vector<Vec3f> m_data;

};