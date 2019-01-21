#pragma once
#include "renderPass/renderPass.h"
#include <glad/glad.h>

class RayPass : public RenderPass {
public:
	RayPass(std::string name,
			RenderPassType type,
			std::initializer_list<PipelineIO> inputs,
			std::initializer_list<PipelineIO> outputs);

	virtual void render() = 0;
	// UI
	virtual bool guiEdit() = 0;

};