#include "buffer.h"
#include <cassert>

Buffer::Buffer(PipelineIO type, uint width, uint height, PipelineHW hw)
	: m_type(type)
	, m_width(width)
	, m_height(height)
	, m_hardware(hw)
{
}

