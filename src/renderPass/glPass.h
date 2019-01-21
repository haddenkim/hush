#pragma once
#include "renderPass/renderPass.h"
#include <glad/glad.h>

class GlPass : public RenderPass {
public:
	GlPass(std::string name,
		   RenderPassType type,
		   std::initializer_list<PipelineIO> inputs,
		   std::initializer_list<PipelineIO> outputs);

	virtual void render() = 0;
	// UI
	virtual bool guiEdit() = 0;

protected:
	// setup helpers
	virtual void setupShader() = 0;
	virtual void setupFBO() = 0;

	GLuint m_shader;
	GLuint m_FBO;
};