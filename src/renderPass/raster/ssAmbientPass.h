#pragma once
#include "renderPass/glPass.h"

class Pipeline;
class GpuBuffer;

class SsAmbientPass : public GlPass {
public:
	SsAmbientPass(GpuBuffer* inColoBuffer,
				  GpuBuffer* matAmbientBuffer,
				  GpuBuffer* matDiffuseBuffer,
				  GLuint canvasVAO,
				  GpuBuffer* outColoBuffer);

	void render() override;

	// UI
	bool guiEdit() override;
	float m_ambientIntensity;
	bool m_useDiffuse;

protected:
	// setup helpers
	void setupShader() override;
	void setupFBO() override;

	// inputs
	GpuBuffer* m_inColorBuffer;
	GpuBuffer* m_matAmbientBuffer;
	GpuBuffer* m_matDiffuseBuffer;

	// data
	const GLuint m_canvasVAO;

	// outputs
	GpuBuffer* m_outColorBuffer;
};