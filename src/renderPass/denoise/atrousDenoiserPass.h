#pragma once
#include "renderPass/glPass.h"

class Pipeline;
class GpuBuffer;

class AtrousDenoiserPass : public GlPass {
public:
	AtrousDenoiserPass(GpuBuffer* rtColorBuffer,
					   GpuBuffer* positionBuffer,
					   GpuBuffer* normalBuffer,
					   float resolution,
					   GLuint canvasVAO,
					   GpuBuffer* colorBuffer);

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

	// data
	const float m_resolution;
	const GLuint m_canvasVAO;
	GLuint m_tempColorBuffer;

	// outputs
	GpuBuffer* m_colorBuffer;
};