#pragma once
#include "renderPass/glPass.h"

class Pipeline;
class Buffer;

class SsAmbientPass : public GlPass {
public:
	SsAmbientPass(Pipeline* pipeline);

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
	Buffer* m_matAmbientBuffer;
	Buffer* m_matDiffuseBuffer;

	// data
	GLuint m_canvasVAO;

	// outputs
	Buffer* m_colorBuffer;
};