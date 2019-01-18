#pragma once
#include "common.h"
#include "renderPass/renderPass.h"
#include <glad/glad.h>

class Pipeline;
class BufferManager;

class RenderToScreenPass : public RenderPass {
public:
	RenderToScreenPass(Pipeline* pipeline);

	void render() override;

	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader();

	GLuint m_displayColorShader;
	GLuint m_canvasVAO;

	BufferManager* m_bufferManager;
	uint* m_displayedBufferIndex;
};