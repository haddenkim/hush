#pragma once
#include "renderPass/glPass.h"

class Pipeline;
class GpuBuffer;

class AtrousDenoiserPass : public GlPass {
public:
	AtrousDenoiserPass(Pipeline* pipeline);

	void render() override;

	// UI
	bool guiEdit() override;
	uint m_filterIterations;
	float m_colorSigma;
	float m_positionSigma;
	float m_normalSigma;
	bool m_useOptionalDiffuse;

protected:
	// setup helpers
	void setupShader() override;
	void setupFBO() override;

	// inputs
	GpuBuffer* m_rtColorBuffer;
	GpuBuffer* m_positionBuffer;
	GpuBuffer* m_normalBuffer;
	const float m_resolution;

	// data
	GLuint m_canvasVAO;
	GpuBuffer* m_tempColorBuffer;

	// outputs
	GpuBuffer* m_colorBuffer;
};