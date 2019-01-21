#pragma once
#include "renderPass/glPass.h"

class Pipeline;
class GpuBuffer;

class ToneMapPass : public GlPass {
public:
	ToneMapPass(GpuBuffer* inColorBuffer,
				GLuint canvasVAO,
				GpuBuffer* outColorBuffer);

	void render() override;

	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader() override;
	void setupFBO() override;

	// inputs
	GpuBuffer* m_inColorBuffer;

	// data
	const GLuint m_canvasVAO;

	// outputs
	GpuBuffer* m_outColorBuffer;
};