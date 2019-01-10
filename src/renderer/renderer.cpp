#include "renderer.h"
#include "camera/camera.h"
#include "scene/scene.h"
#include "shaders/loadShader.h"

#include <fstream>
#include <glad/glad.h>

Renderer::Renderer(Scene* scene, Camera* camera)
	: m_scene(scene)
	, m_camera(camera)
{
	m_width = m_camera->m_width;
	m_height = m_camera->m_height;

	setupShader();
	setupFBO();
	setupCanvas();
}

void Renderer::renderTonemap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_renderedFBO);
	glBindVertexArray(m_canvasVAO);
	glUseProgram(m_tonemapPass);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_preTonemapImage);

	// draw to the post tone map texture
	glDrawBuffer(GL_COLOR_ATTACHMENT1); // draws to post tonemap texture
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glDrawBuffer(GL_COLOR_ATTACHMENT0); // reset FBO to pre-tonemap texture
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // draw to the window FBO
	glBindVertexArray(m_canvasVAO);		  // draw on the canvas quad
	glUseProgram(m_displayTexPass);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebufferTextures[m_displayFramebuffer]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setupShader()
{
	/* draw selected texture */
	m_displayTexPass = LoadShader::createGlProgram("drawTexture.vs",
												   "drawTexture.fs");
	glUseProgram(m_displayTexPass);
	glUniform1i(glGetUniformLocation(m_displayTexPass, "displayTex"), 0);

	/* draw texture with tone map */
	m_tonemapPass = LoadShader::createGlProgram("drawTexture.vs",
												"drawTextureTonemap.fs");
	glUseProgram(m_tonemapPass);
	glUniform1i(glGetUniformLocation(m_tonemapPass, "displayTex"), 0);

	// unbind
	glUseProgram(0);
}

void Renderer::setupFBO()
{
	m_preTonemapImage = setupOutputTexture();
	m_postTonemapImage = setupOutputTexture();

	// FBO
	glGenFramebuffers(1, &m_renderedFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_renderedFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_preTonemapImage, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_postTonemapImage, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	addFrameBufferTexture(m_postTonemapImage, "Final Render");
	addFrameBufferTexture(m_preTonemapImage, "Render wo Tone Map");
}

GLuint Renderer::setupOutputTexture(GLenum format)
{
	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return texId;
}

void Renderer::setupCanvas()
{
	// clang-format off
	// x, y, z, u, v
	float canvasVerticesAndUvs[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // up left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // down left
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // up right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // down right
    };
	// clang-format on

	GLuint canvasVBO;

	glGenVertexArrays(1, &m_canvasVAO);
	glGenBuffers(1, &canvasVBO);

	glBindVertexArray(m_canvasVAO);
	glBindBuffer(GL_ARRAY_BUFFER, canvasVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVerticesAndUvs), &canvasVerticesAndUvs, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// UV
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// unbind
	glBindVertexArray(0);
}

void Renderer::addFrameBufferTexture(GLuint glId, std::string name)
{
	m_framebufferTextures.push_back(glId);
	m_framebufferNames.push_back(name);
	m_framebufferAllowed.push_back(true);
}