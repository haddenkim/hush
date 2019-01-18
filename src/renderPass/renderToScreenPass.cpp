#include "renderToScreenPass.h"

#include "app/viewer.h"
#include "pipeline/pipeline.h"
#include "pipelineBuffer/bufferManager.h"
#include "shaders/loadShader.h"

RenderToScreenPass::RenderToScreenPass(Pipeline* pipeline)
	: RenderPass("Draw To Screen",
				 TO_SCREEN,
				 {}, // inputs
				 {}) // outputs
	, m_canvasVAO(pipeline->m_canvasVAO)
	, m_bufferManager(&pipeline->m_bufferManager)
	, m_displayedBufferIndex(&pipeline->m_displayedBufferIndex)
{
	setupShader();

}

void RenderToScreenPass::render()
{
	// CODEHERE - try to do this viewport resizing in viewer
	/* render selected framebuffer to final window's framebuffer */
	// collect window size - for resizing and mac retina
	int viewPortW, viewPortH;
	glfwGetFramebufferSize(Viewer::Window, &viewPortW, &viewPortH);
	int viewPortX = (viewPortW - viewPortH) / 2;
	glViewport(viewPortX, 0, viewPortH, viewPortH);

	// gets the buffer to display
	GLuint texId = m_bufferManager->getGlTextureId(*m_displayedBufferIndex);

	// selects the shader to draw
	glUseProgram(m_displayColorShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // draw to the window FBO
	glBindVertexArray(m_canvasVAO);		  // draw on the canvas quad

	// pass to GPU
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);

	// draw to canvas
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glBindVertexArray(0);
	glUseProgram(0);
}

// setup helpers
void RenderToScreenPass::setupShader()
{
	m_displayColorShader = LoadShader::createGlProgram("drawTexture.vs",
													   "drawTexture.fs");

	glUseProgram(m_displayColorShader);
	glUniform1i(glGetUniformLocation(m_displayColorShader, "displayTex"), 0);

	// unbind
	glUseProgram(0);
}
