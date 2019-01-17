#include "pipeline.h"
#include "camera/camera.h"
#include "pipelineBuffer/buffer.h"
#include "renderPass/renderPass.h"
#include "shaders/loadShader.h"
#include <glad/glad.h>

Pipeline::Pipeline(Scene* scene, Camera* camera, std::initializer_list<RenderPassType> passes)
	: m_scene(scene)
	, m_camera(camera)
{
	m_width = m_camera->m_width;
	m_height = m_camera->m_height;

	setupShader();
	setupCanvas();

	for (RenderPassType passType : passes) {
		addPass(passType);
	}

	// initial settings
	selectDrawBuffer(m_buffers.size() - 1); // draw the final buffer
											// selectDrawBuffer(3);
}

void Pipeline::render()
{
	for (RenderPass* pass : m_passes) {
		pass->render();
	}
}

void Pipeline::drawToScreen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // draw to the window FBO
	glBindVertexArray(m_canvasVAO);		  // draw on the canvas quad
	glUseProgram(m_displayColorShader);

	// gets the buffer to display
	Buffer* displayBuffer = m_buffers[m_displayedBufferIndex];
	displayBuffer->prepareToDisplay();

	// pass to GPU
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displayBuffer->m_glId);

	// draw to canvas
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind, reset
	glBindVertexArray(0);
	glUseProgram(0);
}

void Pipeline::selectDrawBuffer(uint index)
{
	assert(index < m_buffers.size());
	m_displayedBufferIndex = index;
}

void Pipeline::addPass(RenderPassType type, int position)
{
	RenderPass* newPass = RenderPass::create(type, this);

	if (position < 0) {
		m_passes.push_back(newPass);

	}
	// CODEHERE add passes in position
	else {
		assert(!"Not yet implemented");
	}
}

Buffer* Pipeline::getOrCreateBuffer(PipelineIO type, bool isCPU)
{
	int index = std::find(m_bufferTypes.begin(), m_bufferTypes.end(), type) - m_bufferTypes.begin();

	if (index < m_bufferTypes.size()) {
		return m_buffers[index];

	} else {
		//
		m_bufferTypes.push_back(type);

		Buffer* buffer = Buffer::create(type, m_width, m_height, isCPU);

		m_buffers.emplace_back(buffer);

		return buffer;
	}
}

void Pipeline::setupShader()
{
	/* draw selected texture */
	m_displayColorShader = LoadShader::createGlProgram("drawTexture.vs",
													   "drawTexture.fs");
	glUseProgram(m_displayColorShader);
	glUniform1i(glGetUniformLocation(m_displayColorShader, "displayTex"), 0);

	// unbind
	glUseProgram(0);
}

void Pipeline::setupCanvas()
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
