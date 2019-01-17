#include "common.h"
#include "pipelineBuffer/buffer.h"
#include <vector>

class SpectrumBuffer : public Buffer {
public:
	SpectrumBuffer(PipelineIO type, uint width, uint height);

	void prepareToDisplay() override;

	std::vector<Spectrum> m_data;

};