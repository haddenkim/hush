#include "renderToScreenPass.h"

#include "app/viewer.h"
#include "pipeline/pipeline.h"
#include "shaders/loadShader.h"

#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/bufferSync.h"
#include "pipelineBuffer/cpuBuffer.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "pipelineBuffer/spectrumBuffer.h"

RenderToScreenPass::RenderToScreenPass(Buffer** displayBuffer,
									   GLuint canvasVAO)
	: RenderPass("Draw To Screen",
				 TO_SCREEN,
				 {}, // inputs
				 {}) // outputs
	, m_displayBuffer(displayBuffer)
	, m_canvasVAO(canvasVAO)
{
	setupShader();
	setupCpuTexture();
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

	// selects the shader to draw

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // draw to the window FBO
	glBindVertexArray(m_canvasVAO);		  // draw on the canvas quad

	// get and pre-process buffer
	Buffer* displayBuffer = m_displayBuffer[0];
	GLuint displayTex;

	if (displayBuffer->m_hardware == GPU) {
		displayTex = static_cast<GpuBuffer*>(displayBuffer)->m_texId;
	} else {
		static_cast<CpuBuffer*>(displayBuffer)->passToGPU(m_displayCPUBufferTexId);
		displayTex = m_displayCPUBufferTexId;
	}

	// select and use shader
	glUseProgram(selectShader());

	// uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayTex);

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

void RenderToScreenPass::setupCpuTexture()
{
	glGenTextures(1, &m_displayCPUBufferTexId);
	glBindTexture(GL_TEXTURE_2D, m_displayCPUBufferTexId);
	// paramaters and data will be set by CpuBuffer.passToGPU calls
}

GLuint RenderToScreenPass::selectShader()
{
	// CODEHERE - different shaders to adjust various data types (ex. normals -> RGB representation)
	return m_displayColorShader;
}