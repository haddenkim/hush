#include "renderPass.h"
#include "renderPass/rasterGBufferPass.h"
#include "renderPass/rtFullGiPass.h"
#include "renderPass/ssAmbientPass.h"
#include "renderPass/ssLightPass.h"

#include "pipelineBuffer/buffer.h"
#include "pipeline/pipeline.h"

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
		pass = new RasterGBufferPass(pipeline->m_scene,
									 pipeline->m_camera,
									 pipeline->getOrCreateBuffer(G_POSITION),
									 pipeline->getOrCreateBuffer(G_NORMAL),
									 pipeline->getOrCreateBuffer(G_MAT_AMBIENT),
									 pipeline->getOrCreateBuffer(G_MAT_DIFFUSE),
									 pipeline->getOrCreateBuffer(G_MAT_SPECULAR));
		break;

	case SS_DIRECT_LIGHT:
		pass = new SsLightPass(pipeline->m_scene,
							   pipeline->m_camera,
							   pipeline->getOrCreateBuffer(G_POSITION),
							   pipeline->getOrCreateBuffer(G_NORMAL),
							   pipeline->getOrCreateBuffer(G_MAT_DIFFUSE),
							   pipeline->getOrCreateBuffer(G_MAT_SPECULAR),
							   pipeline->getOrCreateBuffer(COLOR),
							   pipeline->m_canvasVAO);
		break;

	case SS_AMBIENT:
		pass = new SsAmbientPass(pipeline->getOrCreateBuffer(COLOR),
								 pipeline->getOrCreateBuffer(G_MAT_AMBIENT),
								 pipeline->getOrCreateBuffer(G_MAT_DIFFUSE),
								 pipeline->m_canvasVAO);
		break;

	case RT_FULL_GI:
		pass = new RtFullGiPass(pipeline->m_scene,
								pipeline->m_camera,
								pipeline->getOrCreateBuffer(RT_COLOR, true));
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
