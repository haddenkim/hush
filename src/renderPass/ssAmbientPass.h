#pragma once
#include "renderPass/glPass.h"

class Buffer;

class SsAmbientPass : public GlPass {
public:
	SsAmbientPass(Buffer* colorBuffer,
				  Buffer* matAmbientBuffer,
				  Buffer* matDiffuseBuffer,
				  GLuint canvasVAO);

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