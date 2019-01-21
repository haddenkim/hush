#pragma once

#include "common.h"
#include "gui/iGuiEditable.h"
#include "pipeline/pipelineIO.h"
#include "renderPass/renderPassType.h"
#include <initializer_list>
#include <string>

class Pipeline;

class RenderPass : public IGuiEditable {
public:
	RenderPass(std::string name,
			   RenderPassType type,
			   std::initializer_list<PipelineIO> inputs,
			   std::initializer_list<PipelineIO> outputs);

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