#pragma once

#include "common.h"
#include "gui/iGuiEditable.h"
#include "pipeline/pipelineIO.h"
#include <initializer_list>
#include <string>

class Pipeline;

enum RenderPassType {
	// rasterization
	RASTER_GBUFFER,

	// ray trace
	RT_FULL_GI,

	// screen space
	SS_DIRECT_LIGHT,
	SS_AMBIENT,

	// denoising
	DENOISE_ATROUS,

	// post processing
	PP_TONE_MAP,

	// To screen
	TO_SCREEN

};

class RenderPass : public IGuiEditable {
public:
	RenderPass(std::string name,
			   RenderPassType type,
			   std::initializer_list<PipelineIO> inputs,
			   std::initializer_list<PipelineIO> outputs);
	static RenderPass* create(RenderPassType type, Pipeline* pipeline);
	static PipelineIOMask createIOMask(std::initializer_list<PipelineIO> ios);

	virtual void render() = 0;

	// UI
	virtual bool guiEdit() = 0;

	// state
	const std::string m_name;
	const RenderPassType m_type;
	const PipelineIOMask m_inputs;
	const PipelineIOMask m_outputs;
};