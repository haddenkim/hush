#pragma once
#include "common.h"
#include <glad/glad.h>

class PtRenderer;

class AtrousDenoiser {
public:
	AtrousDenoiser(PtRenderer* ptRenderer);

	void render();

	// settings
	uint m_filterIterations;
	float m_colorSigma;
	float m_positionSigma;
	float m_normalSigma;
	bool m_useOptionalDiffuse;

protected:
	// renderer pointer
	PtRenderer* m_ptRenderer;

	// openGL
	GLuint m_atrousPass;
	GLuint m_diffusePass;
	GLuint m_denoiseFBO;
	GLuint m_texOut0;
	GLuint m_texOut1;

	// setup helper
	void setupShader();
	void setupFBO();
};