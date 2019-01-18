#include "cpuBuffer.h"

CpuBuffer::CpuBuffer(PipelineIO type, uint width, uint height)
	: Buffer(type, width, height, CPU)
{
}
