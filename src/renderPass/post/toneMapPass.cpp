#include "toneMapPass.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "shaders/loadShader.h"

ToneMapPass::ToneMapPass(GpuBuffer* inColoBuffer,
						 GLuint canvasVAO,
						 GpuBuffer* outColoBuffer)
	: GlPass("Tone Map",
			 RASTER_GBUFFER,
			 { COLOR }, // inputs
			 { COLOR }) // outputs
	, m_inColorBuffer(inColoBuffer)
	, m_canvasVAO(canvasVAO)
	, m_outColorBuffer(outColoBuffer)

{
	setupShader();
	setupFBO();
}

void ToneMapPass::render()
{
	// use this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// use program
	glUseProgram(m_shader);
	glBindVertexArray(m_canvasVAO);

	// load uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_inColorBuffer->m_texId);

	// draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// setup helpers
void ToneMapPass::setupShader()
{
	m_shader = LoadShader::createGlProgram("drawTexture.vs",
										   "ssToneMap.fs");

	// uniforms
	glUseProgram(m_shader);
	glUniform1i(glGetUniformLocation(m_shader, "inColorTex"), 0);

	// unbind
	glUseProgram(0);
}

void ToneMapPass::setupFBO()
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_outColorBuffer->m_texId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
