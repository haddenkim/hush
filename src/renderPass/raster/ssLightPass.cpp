#include "ssLightPass.h"
#include "camera/camera.h"
#include "light/light.h"
#include "pipelineBuffer/buffer.h"
#include "scene/scene.h"
#include "shaders/loadShader.h"

SsLightPass::SsLightPass(Scene* scene,
						 Camera* camera,
						 Buffer* positionBuffer,
						 Buffer* normalBuffer,
						 Buffer* matDiffuseBuffer,
						 Buffer* matSpecularBuffer,

						 Buffer* colorBuffer,
						 GLuint canvasVAO)

	: GlPass("Screen Space Direct Lighting",
			 RASTER_GBUFFER,
			 { CAM_POSITION, LIGHT_POSITION, LIGHT_INTENSITY, G_POSITION, G_NORMAL, G_MAT_DIFFUSE, G_MAT_SPECULAR }, // inputs
			 { COLOR })																								 // outputs
	, m_scene(scene)
	, m_camera(camera)
	, m_positionBuffer(positionBuffer)
	, m_normalBuffer(normalBuffer)
	, m_matDiffuseBuffer(matDiffuseBuffer)
	, m_matSpecularBuffer(matSpecularBuffer)
	, m_colorBuffer(colorBuffer)
	, m_canvasVAO(canvasVAO)
{
	setupShader();
	setupFBO();
}

void SsLightPass::render()
{
	// use this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// use lighting program
	glUseProgram(m_shader);

	// blending
	// CODEHERE - accurate blending function
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindVertexArray(m_canvasVAO);

	// load uniforms
	glUniform3fv(glGetUniformLocation(m_shader, "cameraPosition"), 1, &m_camera->m_position[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionBuffer->m_glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalBuffer->m_glId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_matDiffuseBuffer->m_glId);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_matSpecularBuffer->m_glId);

	// for each enabled light
	for (Light* light : m_scene->m_enabledLightList) {

		// treat every light source as point light
		Spectrum intensity = light->m_power * ONE_OVER_PI * 0.25f;

		// pass to shader uniform
		glUniform3fv(glGetUniformLocation(m_shader, "cameraPosition"), 1, &m_camera->m_position[0]);
		glUniform3fv(glGetUniformLocation(m_shader, "light.position"), 1, &light->m_position[0]);
		glUniform3fv(glGetUniformLocation(m_shader, "light.intensity"), 1, &intensity[0]);

		// draw lighting pass
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// CODEHERE - MESH LIGHTING SHADER, instead of above treating as point light above
	// for each mesh light

	// unbind, reset
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// setup helpers
void SsLightPass::setupShader()
{
	m_shader = LoadShader::createGlProgram("drawTexture.vs",
										   "ssLight.fs");

	// uniforms
	glUseProgram(m_shader);
	glUniform1i(glGetUniformLocation(m_shader, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(m_shader, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(m_shader, "gDiffuse"), 2);
	glUniform1i(glGetUniformLocation(m_shader, "gSpecular"), 3);

	// unbind
	glUseProgram(0);
}

void SsLightPass::setupFBO()
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
