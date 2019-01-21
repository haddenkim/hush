#pragma once
#include "common.h"
#include "renderPass/renderPass.h"
#include <glad/glad.h>

class Buffer;

class RenderToScreenPass : public RenderPass {
public:
	RenderToScreenPass(Buffer** displayBuffer,
					   GLuint canvasVAO);

	void render() override;

	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader();
	void setupCpuTexture();

	// render helpers
	GLuint selectShader();

	GLuint m_displayCPUBufferTexId;
	GLuint m_displayColorShader;
	const GLuint m_canvasVAO;

	Buffer** m_displayBuffer; // pointer to the pipeline buffer pointer
};