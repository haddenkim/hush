#include "renderPass.h"
#include "renderPass/denoise/atrousDenoiserPass.h"
#include "renderPass/raster/rasterGBufferPass.h"
#include "renderPass/raster/ssAmbientPass.h"
#include "renderPass/raster/ssLightPass.h"
#include "renderPass/ray/pathTracePass.h"
#include "renderPass/renderToScreenPass.h"

#include "pipeline/pipeline.h"
#include "pipelineBuffer/buffer.h"

RenderPass::RenderPass(std::string name,
					   RenderPassType type,
					   std::initializer_list<PipelineIO> inputs,
					   std::initializer_list<PipelineIO> outputs)
	: m_name(name)
	, m_type(type)
	, m_inputs(createIOMask(inputs))
	, m_outputs(createIOMask(outputs))
{
}


PipelineIOMask RenderPass::createIOMask(std::initializer_list<PipelineIO> ios)
{
	PipelineIOMask mask;
	for (PipelineIO io : ios) {
		mask.set(io);
	}
	return mask;
}
