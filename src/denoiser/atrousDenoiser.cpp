#include "atrousDenoiser.h"
#include "renderer/ptRenderer.h"
#include "shaders/loadShader.h"

AtrousDenoiser::AtrousDenoiser(PtRenderer* ptRenderer)
	: m_ptRenderer(ptRenderer)
{
	setupShader();
	setupFBO();

	// initial settings
	m_filterIterations = 5;
	m_colorSigma = 1.f;
	m_positionSigma = 1.f;
	m_normalSigma = 0.1f;
	m_useOptionalDiffuse = true;
}

void AtrousDenoiser::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_denoiseFBO);
	glBindVertexArray(m_ptRenderer->m_canvasVAO);

	// clear
	glClearColor(0.4, 0.4f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// atrous pass
	glUseProgram(m_atrousPass);

	// input textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_ptRenderer->m_gBufferTex[1]); // pt position
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_ptRenderer->m_gBufferTex[2]); // pt normal

	// initial read/write color textures
	GLuint colorTexInput = m_ptRenderer->m_gBufferTex[0]; // pt color
	GLuint colorTexOutput = m_texOut0;

	for (uint i = 0; i < m_filterIterations;) {
		// send uniforms
		// filter algorithm weights
		float scale = powf(2.f, -(float)i);
		float stepwidth = powf(2.f, (float)i);

		float colSigmaI = scale * m_colorSigma;
		float colPhi = colSigmaI * colSigmaI;

		float posSigmaI = scale * m_positionSigma;
		float posPhi = posSigmaI * posSigmaI;

		float norSigmaI = scale * m_normalSigma;
		float norPhi = norSigmaI * norSigmaI;

		// setup per iteration uniforms
		glUniform1f(glGetUniformLocation(m_atrousPass, "resolution"), (float)m_ptRenderer->m_width);
		glUniform1f(glGetUniformLocation(m_atrousPass, "colorPhi"), colPhi);
		glUniform1f(glGetUniformLocation(m_atrousPass, "positionPhi"), posPhi);
		glUniform1f(glGetUniformLocation(m_atrousPass, "normalPhi"), norPhi);
		glUniform1f(glGetUniformLocation(m_atrousPass, "stepwidth"), stepwidth);

		// set read and write textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexInput);
		glDrawBuffer(colorTexOutput == m_texOut0 ? GL_COLOR_ATTACHMENT0 : GL_COLOR_ATTACHMENT1); // toggle between output 0 and 1

		// draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// swap read and write attachments
		if (i == 0)
			colorTexInput = m_texOut1;
		if (++i < m_filterIterations)
			std::swap(colorTexInput, colorTexOutput);
	}

	// diffuse pass
	glBindFramebuffer(GL_FRAMEBUFFER, m_ptRenderer->m_renderedFBO);

	if (m_useOptionalDiffuse) {
		glUseProgram(m_diffusePass);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_ptRenderer->m_gBufferTex[3]);
	} else {

		glUseProgram(m_ptRenderer->m_displayTexPass);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexOutput);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void AtrousDenoiser::setupShader()
{

	m_atrousPass = LoadShader::createGlProgram("src/shaders/drawTexture.vs",
											   "src/shaders/denoiseAtrous.fs");

	glUseProgram(m_atrousPass);

	glUniform1i(glGetUniformLocation(m_atrousPass, "gColor"), 0);
	glUniform1i(glGetUniformLocation(m_atrousPass, "gPosition"), 1);
	glUniform1i(glGetUniformLocation(m_atrousPass, "gNormal"), 2);

	//
	m_diffusePass = LoadShader::createGlProgram("src/shaders/drawTexture.vs",
												"src/shaders/denoiseAtrousDiffuse.fs");
	glUseProgram(m_diffusePass);

	glUniform1i(glGetUniformLocation(m_diffusePass, "dColor"), 0);
	glUniform1i(glGetUniformLocation(m_diffusePass, "gDiffuse"), 1);

	/* unbind */
	glUseProgram(0);
}

void AtrousDenoiser::setupFBO()
{
	m_texOut0 = m_ptRenderer->setupOutputTexture();
	m_texOut1 = m_ptRenderer->setupOutputTexture();

	glGenFramebuffers(1, &m_denoiseFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_denoiseFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texOut0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_texOut1, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
