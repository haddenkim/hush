#pragma once
#include "pipeline/pipelineIO.h"

class Buffer;

class BufferSync {
public:
	BufferSync(Buffer* fromBuffer, Buffer* toBuffer);

	void sync();

	// data
	Buffer* m_fromBuffer;
	Buffer* m_toBuffer;
	PipelineHW m_toHardware;
};