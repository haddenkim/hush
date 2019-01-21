#include "renderPass/denoise/atrousDenoiserPass.h"
#include "pipeline/pipeline.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "shaders/loadShader.h"

AtrousDenoiserPass::AtrousDenoiserPass(GpuBuffer* rtColorBuffer,
									   GpuBuffer* positionBuffer,
									   GpuBuffer* normalBuffer,
									   float resolution,
									   GLuint canvasVAO,
									   GpuBuffer* colorBuffer)
	: GlPass("Atrous Denoiser",
			 DENOISE_ATROUS,
			 { COLOR, G_POSITION, G_NORMAL }, // inputs
			 { COLOR })						  // outputs
	, m_rtColorBuffer(rtColorBuffer)
	, m_positionBuffer(positionBuffer)
	, m_normalBuffer(normalBuffer)
	, m_resolution(resolution)
	, m_canvasVAO(canvasVAO)
	, m_colorBuffer(colorBuffer)

{
	setupShader();
	setupFBO();

	// initial settings
	m_filterIterations = 4;
	m_colorSigma = 1.f;
	m_positionSigma = 1.f;
	m_normalSigma = 0.1f;
	m_useOptionalDiffuse = false;
}

void AtrousDenoiserPass::render()
{
	// use this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// use program
	glUseProgram(m_shader);
	glBindVertexArray(m_canvasVAO);

	// input textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_positionBuffer->m_texId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_normalBuffer->m_texId);

	// initial read/write color textures
	GLuint colorTexInput = m_rtColorBuffer->m_texId; // pt color

	// ensure that the last draw occurs on the output texture
	GLuint colorTexOutput = m_filterIterations % 2 == 0 ? m_tempColorBuffer :  m_colorBuffer->m_texId;

	for (uint i = 0; i < m_filterIterations;) {
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
		glUniform1f(glGetUniformLocation(m_shader, "resolution"), m_resolution);
		glUniform1f(glGetUniformLocation(m_shader, "colorPhi"), colPhi);
		glUniform1f(glGetUniformLocation(m_shader, "positionPhi"), posPhi);
		glUniform1f(glGetUniformLocation(m_shader, "normalPhi"), norPhi);
		glUniform1f(glGetUniformLocation(m_shader, "stepwidth"), stepwidth);

		// set read and write textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexInput);
		glDrawBuffer(colorTexOutput == m_tempColorBuffer ? GL_COLOR_ATTACHMENT1 : GL_COLOR_ATTACHMENT0); // toggle between output 0 and 1

		// draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// swap read and write attachments
		if (i == 0)
			colorTexInput = colorTexOutput == m_tempColorBuffer ? m_colorBuffer->m_texId : m_tempColorBuffer; // will immediately be flipped below
		if (++i < m_filterIterations)
			std::swap(colorTexInput, colorTexOutput);
	}

	assert(colorTexOutput != m_tempColorBuffer);

	// unbind, reset
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// setup helpers
void AtrousDenoiserPass::setupShader()
{
	m_shader = LoadShader::createGlProgram("drawTexture.vs",
										   "denoiseAtrous.fs");

	glUseProgram(m_shader);

	glUniform1i(glGetUniformLocation(m_shader, "rtColor"), 0);
	glUniform1i(glGetUniformLocation(m_shader, "gPosition"), 1);
	glUniform1i(glGetUniformLocation(m_shader, "gNormal"), 2);

	// unbind
	glUseProgram(0);
}

void AtrousDenoiserPass::setupFBO()
{
	// setup the temp color buffer
	glGenTextures(1, &m_tempColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_tempColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_colorBuffer->m_width, m_colorBuffer->m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind

	// setup framebuffer
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer->m_texId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_tempColorBuffer, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
