#include "renderPass/ssAmbientPass.h"
#include "pipelineBuffer/buffer.h"
#include "shaders/loadShader.h"

SsAmbientPass::SsAmbientPass(Buffer* colorBuffer,
							 Buffer* matAmbientBuffer,
							 Buffer* matDiffuseBuffer,
							 GLuint canvasVAO)
	: GlPass("Screen Space Ambient Lighting",
			 RASTER_GBUFFER,
			 { COLOR, G_MAT_AMBIENT, G_MAT_DIFFUSE }, // inputs
			 { COLOR })								  // outputs
	, m_colorBuffer(colorBuffer)
	, m_matAmbientBuffer(matAmbientBuffer)
	, m_matDiffuseBuffer(matDiffuseBuffer)
	, m_canvasVAO(canvasVAO)
{
	setupShader();
	setupFBO();

	// initial settings
	m_ambientIntensity = 0.1f;
	m_useDiffuse = false;
}

void SsAmbientPass::render()
{
	// use this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// glClearColor(0.f, 0.f, 0.f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT);

	// use program
	glUseProgram(m_shader);

	// blending
	// CODEHERE - accurate blending function
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindVertexArray(m_canvasVAO);

	// load uniforms
	glUniform1f(glGetUniformLocation(m_shader, "ambientIntensity"), m_ambientIntensity);
	glUniform1i(glGetUniformLocation(m_shader, "useDiffuse"), m_useDiffuse ? 1 : 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_matAmbientBuffer->m_glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_matDiffuseBuffer->m_glId);

	// draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// setup helpers
void SsAmbientPass::setupShader()
{
	m_shader = LoadShader::createGlProgram("drawTexture.vs",
										   "ssAmbient.fs");

	// uniforms
	glUseProgram(m_shader);
	glUniform1i(glGetUniformLocation(m_shader, "gAmbient"), 0);
	glUniform1i(glGetUniformLocation(m_shader, "gDiffuse"), 1);

	// unbind
	glUseProgram(0);
}

void SsAmbientPass::setupFBO()
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer->m_glId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
