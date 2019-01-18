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

RenderPass* RenderPass::create(RenderPassType type, Pipeline* pipeline)
{

	RenderPass* pass;

	switch (type) {
	case RASTER_GBUFFER:
		pass = new RasterGBufferPass(pipeline);
		break;

	case SS_DIRECT_LIGHT:
		pass = new SsLightPass(pipeline);
		break;

	case SS_AMBIENT:
		pass = new SsAmbientPass(pipeline);
		break;

	case RT_FULL_GI:
		pass = new PathTracePass(pipeline);
		break;

	case DENOISE_ATROUS:
		pass = new AtrousDenoiserPass(pipeline);

	case TO_SCREEN:
		pass = new RenderToScreenPass(pipeline);
		break;

	default:
		assert(!"The default case of switch was reached.");
		break;
	}

	return pass;
}

PipelineIOMask RenderPass::createIOMask(std::initializer_list<PipelineIO> ios)
{
	PipelineIOMask mask;
	for (PipelineIO io : ios) {
		mask.set(io);
	}
	return mask;
}
