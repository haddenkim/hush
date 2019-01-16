#include "glPass.h"

GlPass::GlPass(std::string name,
			   RenderPassType type,
			   std::initializer_list<PipelineIO> inputs,
			   std::initializer_list<PipelineIO> outputs)
	: RenderPass(name, type, inputs, outputs)
{
}