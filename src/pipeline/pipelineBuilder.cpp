#include "pipelineBuilder.h"
#include "pipeline/pipeline.h"
#include "pipelineBuffer/buffer.h"

#include "renderPass/denoise/atrousDenoiserPass.h"
#include "renderPass/post/toneMapPass.h"
#include "renderPass/raster/rasterGBufferPass.h"
#include "renderPass/raster/ssAmbientPass.h"
#include "renderPass/raster/ssLightPass.h"
#include "renderPass/ray/pathTracePass.h"
#include "renderPass/renderToScreenPass.h"

#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/bufferSync.h"
#include "pipelineBuffer/cpuBuffer.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "pipelineBuffer/spectrumBuffer.h"
#include "pipelineBuffer/vec3fBuffer.h"

PipelineBuilder::PipelineBuilder(Pipeline& pipeline)
	: m_pipeline(pipeline)
{
}

bool PipelineBuilder::createPipeline(const BuildPipeline& buildPipeline)
{
	// set pipeline properties
	m_pipeline.m_width = buildPipeline.width;
	m_pipeline.m_height = buildPipeline.height;
	m_pipeline.m_scene = buildPipeline.scene;
	m_pipeline.m_camera = buildPipeline.camera;

	// for each stage in pipeline
	for (const BuildStage& buildStage : buildPipeline.stages) {
		// index of next stage
		uint stageIndex = m_pipeline.m_stages.size();

		// add an empty pipeline stage
		m_pipeline.m_stages.emplace_back();

		// retrieve reference to new stage
		PipelineStage& pipelineStage = m_pipeline.m_stages.back();

		// create the stage
		bool stageSuccess = createStage(buildStage, stageIndex, pipelineStage);

		// return false if failed to create stage
		if (!stageSuccess) {
			printPipeline();

			assert(!"pipeline build failed");
			return false;
		}
	}

	printPipeline();
	return true;
}

bool PipelineBuilder::createStage(const BuildStage& buildStage, uint stageIndex, PipelineStage& pipelineStage)
{
	// create empty pipeline pass
	pipelineStage.m_passes.emplace_back();

	// retrieve reference to new pass
	PipelinePass& pipelinePass = pipelineStage.m_passes.back();

	// for each pass in stage
	for (const BuildPass& buildPass : buildStage.passes) {

		// create the pass
		bool passSuccess = createPass(buildPass, stageIndex, pipelinePass);

		// return false if failed to create pass
		if (!passSuccess) {
			assert(!"stage build failed");
			return false;
		}
	}

	return true;
}

bool PipelineBuilder::createPass(const BuildPass& buildPass, uint stageIndex, PipelinePass& pipelinePass)
{
	// for each link in pass
	for (const BuildLink& buildLink : buildPass.links) {
		// create empty pipeline link
		pipelinePass.m_links.emplace_back();

		// retrieve reference to new link
		PipelineIOLink& pipelineLink = pipelinePass.m_links.back();

		// create the link
		bool linkSuccess = createLink(buildLink, pipelineLink);

		// return false if failed to create link
		if (!linkSuccess) {
			assert(!"pass build failed");
			return false;
		}
	}

	// create the pass
	switch (buildPass.type) {
	case RASTER_GBUFFER:
		pipelinePass.m_renderPass = new RasterGBufferPass(m_pipeline.m_scene,
														  m_pipeline.m_camera,
														  static_cast<GpuBuffer*>(createOutputBuffer(GPU, G_POSITION, pipelinePass)),
														  static_cast<GpuBuffer*>(createOutputBuffer(GPU, G_NORMAL, pipelinePass)),
														  static_cast<GpuBuffer*>(createOutputBuffer(GPU, G_MAT_AMBIENT, pipelinePass)),
														  static_cast<GpuBuffer*>(createOutputBuffer(GPU, G_MAT_DIFFUSE, pipelinePass)),
														  static_cast<GpuBuffer*>(createOutputBuffer(GPU, G_MAT_SPECULAR, pipelinePass)));
		break;
	case SS_DIRECT_LIGHT:
		pipelinePass.m_renderPass = new SsLightPass(m_pipeline.m_scene,
													m_pipeline.m_camera,
													static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_POSITION, stageIndex, pipelinePass)),
													static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_NORMAL, stageIndex, pipelinePass)),
													static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_MAT_DIFFUSE, stageIndex, pipelinePass)),
													static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_MAT_SPECULAR, stageIndex, pipelinePass)),
													m_pipeline.m_canvasVAO,
													static_cast<GpuBuffer*>(createOutputBuffer(GPU, COLOR, pipelinePass)));
		break;

	case SS_AMBIENT:
		pipelinePass.m_renderPass = new SsAmbientPass(static_cast<GpuBuffer*>(requestInputBuffer(GPU, COLOR, stageIndex, pipelinePass)),
													  static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_MAT_AMBIENT, stageIndex, pipelinePass)),
													  static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_MAT_DIFFUSE, stageIndex, pipelinePass)),
													  m_pipeline.m_canvasVAO,
													  static_cast<GpuBuffer*>(createOutputBuffer(GPU, COLOR, pipelinePass)));
		break;

	case RT_FULL_GI:
		pipelinePass.m_renderPass = new PathTracePass(m_pipeline.m_scene,
													  m_pipeline.m_camera,
													  m_pipeline.m_width,
													  m_pipeline.m_height,
													  static_cast<SpectrumBuffer*>(createOutputBuffer(CPU, COLOR, pipelinePass)),
													  static_cast<Vec3fBuffer*>(createOutputBuffer(CPU, G_POSITION, pipelinePass)),
													  static_cast<Vec3fBuffer*>(createOutputBuffer(CPU, G_NORMAL, pipelinePass)),
													  static_cast<SpectrumBuffer*>(createOutputBuffer(CPU, G_MAT_DIFFUSE, pipelinePass)));

		break;
	case DENOISE_ATROUS:
		pipelinePass.m_renderPass = new AtrousDenoiserPass(static_cast<GpuBuffer*>(requestInputBuffer(GPU, COLOR, stageIndex, pipelinePass)),
														   static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_POSITION, stageIndex, pipelinePass)),
														   static_cast<GpuBuffer*>(requestInputBuffer(GPU, G_NORMAL, stageIndex, pipelinePass)),
														   m_pipeline.m_width,
														   m_pipeline.m_canvasVAO,
														   static_cast<GpuBuffer*>(createOutputBuffer(GPU, COLOR, pipelinePass)));
		break;

	case PP_TONE_MAP:
		pipelinePass.m_renderPass = new ToneMapPass(static_cast<GpuBuffer*>(requestInputBuffer(GPU, COLOR, stageIndex, pipelinePass)),
													m_pipeline.m_canvasVAO,
													static_cast<GpuBuffer*>(createOutputBuffer(GPU, COLOR, pipelinePass)));

		break;

	case TO_SCREEN:
		// special case - set pipeline display buffer
		{
			const PipelineIOLink& lastLink = pipelinePass.m_links.back();
			PipelineIO lastType = lastLink.m_types.back();
			m_pipeline.m_displayBuffer = findOutBuffer(GPU, lastType, lastLink.m_fromStage, lastLink.m_fromPass);

			pipelinePass.m_renderPass = new RenderToScreenPass(&m_pipeline.m_displayBuffer,
															   m_pipeline.m_canvasVAO);
		}
		break;

	default:
		assert(!"RenderPassType not implemented in builder");
		return false;
	}

	return true;
}

bool PipelineBuilder::createLink(const BuildLink& buildLink, PipelineIOLink& pipelineLink)
{
	// validate inputs exist
	if (!validateDependencies(buildLink)) {
		assert(!"link build failed");
		return false;
	}

	pipelineLink.m_fromStage = buildLink.fromStage;
	pipelineLink.m_fromPass = buildLink.fromPass;

	for (PipelineIO ioType : buildLink.ioTypes) {
		pipelineLink.m_types.emplace_back(ioType);
	}

	return true;
}

bool PipelineBuilder::validateDependencies(const BuildLink& buildLink)
{
	uint fromStage = buildLink.fromStage;
	uint fromPass = buildLink.fromPass;

	// check stage count
	if (fromStage >= m_pipeline.m_stages.size()) {
		assert(!"dependency validation failed");
		return false;
	}
	const PipelineStage& fromPipelineStage = m_pipeline.m_stages[buildLink.fromStage];

	// check pass count
	if (fromPass >= fromPipelineStage.m_passes.size()) {
		assert(!"dependency validation failed");
		return false;
	}
	const PipelinePass& fromPipelinePass = fromPipelineStage.m_passes[buildLink.fromPass];

	// get reference to buffer list at desired pass in desired stage
	const std::vector<Buffer*>& fromPipelinePassBuffers = fromPipelinePass.m_buffers;

	// verify all iotypes exist
	for (PipelineIO ioType : buildLink.ioTypes) {
		bool isThisTypeFound = false;

		// search the buffer list for this ioType
		for (Buffer* buffer : fromPipelinePassBuffers) {
			if (ioType == buffer->m_type) {
				isThisTypeFound = true;
				break;
			}
		}
		if (!isThisTypeFound) {
			assert(!"dependency validation failed");
			return false;
		}
	}

	return true;
}

Buffer* PipelineBuilder::createOutputBuffer(PipelineHW hw, PipelineIO type, PipelinePass& pipelinePass)
{
	Buffer* buffer = createBuffer(hw, type);

	pipelinePass.m_buffers.push_back(buffer);
	return buffer;
}

Buffer* PipelineBuilder::requestInputBuffer(PipelineHW hw,
											PipelineIO type,
											uint stageIndex,
											PipelinePass& pipelinePass)
{
	// determine output buffer location
	uint fromStage;
	uint fromPass;
	bool isFound = false;
	for (const PipelineIOLink pipelineLink : pipelinePass.m_links) {
		for (PipelineIO ioType : pipelineLink.m_types) {
			if (type == ioType) {
				fromStage = pipelineLink.m_fromStage;
				fromPass = pipelineLink.m_fromPass;
				isFound = true;
				break;
			}
		}
		if (isFound) {
			break;
		}
	}
	assert(isFound); // should not fail because of prior validateDependencies

	// find the output buffer
	Buffer* outBuffer = findOutBuffer(hw, type, fromStage, fromPass);

	if (outBuffer->m_hardware == hw) {
		// same type & hw
		return outBuffer;
	} else {
		// same type but diff hw
		return createBufferSync(hw, type, outBuffer, stageIndex);
	}
}

Buffer* PipelineBuilder::createBufferSync(PipelineHW hw, PipelineIO type, Buffer* fromBuffer, uint stageIndex)
{
	Buffer* toBuffer = createBuffer(hw, type);

	m_pipeline.m_stages[stageIndex].m_bufferSyncs.emplace_back(new BufferSync(fromBuffer, toBuffer));

	return toBuffer;
}

Buffer* PipelineBuilder::createBuffer(PipelineHW hw, PipelineIO type)
{
	Buffer* buffer;

	if (hw == GPU) {
		buffer = new GpuBuffer(type, m_pipeline.m_width, m_pipeline.m_height);
	} else {

		// CPU
		switch (type) {
		case G_MAT_DIFFUSE:
		case COLOR:
			buffer = new SpectrumBuffer(type, m_pipeline.m_width, m_pipeline.m_height);
			break;

		case G_POSITION:
		case G_NORMAL:
			buffer = new Vec3fBuffer(type, m_pipeline.m_width, m_pipeline.m_height);
			break;

		default:
			assert(!"hw + io type not implemented in builder");
			break;
		}
	}

	return buffer;
}

Buffer* PipelineBuilder::findOutBuffer(PipelineHW hw, PipelineIO type, uint fromStage, uint fromPass)
{

	std::vector<Buffer*>& outBuffers = m_pipeline.m_stages[fromStage].m_passes[fromPass].m_buffers;

	// search for matching buffer type
	Buffer* fromBuffer;

	for (Buffer* outBuffer : outBuffers) {
		if (outBuffer->m_type == type) {
			// continue to check for matching hw in case output buffers contain both
			if (outBuffer->m_hardware == hw) {
				// same type & hw
				return outBuffer;
			} else {
				// same type but diff hw
				fromBuffer = outBuffer;
			}
		}
	}

	return fromBuffer;
}

void PipelineBuilder::printPipeline()
{

	printf("Pipeline \n");

	for (size_t s = 0; s < m_pipeline.m_stages.size(); s++) {
		const PipelineStage& stage = m_pipeline.m_stages[s];

		uint numBS = stage.m_bufferSyncs.size();
		uint numRP = stage.m_passes.size();

		printf("Stage %lu\n", s);

		printf("\tBufferSyncs %i\n", numBS);
		for (size_t bs = 0; bs < numBS; bs++) {
			const BufferSync* bufferSync = stage.m_bufferSyncs[bs];
			printf("\t\t%lu: \t", bs);

			Buffer* fromBuffer = bufferSync->m_fromBuffer;
			printf("From: %s %s\t", PipelineHWNames[fromBuffer->m_hardware], PipelineIONames[fromBuffer->m_type]);

			Buffer* toBuffer = bufferSync->m_toBuffer;
			printf("To  : %s %s\n", PipelineHWNames[toBuffer->m_hardware], PipelineIONames[toBuffer->m_type]);
		}

		printf("\tRenderPasses %i\n", numRP);
		for (size_t p = 0; p < numRP; p++) {
			const PipelinePass& pass = stage.m_passes[p];
			const RenderPass* renderPass = pass.m_renderPass;

			printf("\t\t%lu: %s\n", p, renderPass->m_name.c_str());

			for (Buffer* buffer : pass.m_buffers) {
				printf("\t\tOut: %s %s\n", PipelineHWNames[buffer->m_hardware], PipelineIONames[buffer->m_type]);
			}

			for (const PipelineIOLink& pipeLink : pass.m_links) {
				printf("\t\tIn : %i %i ", pipeLink.m_fromStage, pipeLink.m_fromPass);
				for (PipelineIO io : pipeLink.m_types) {
					printf("%s, ", PipelineIONames[io]);
				}
				printf("\n");
			}
		}
	}

	printf("\n");
}