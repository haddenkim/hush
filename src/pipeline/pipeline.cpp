#include "pipeline.h"
#include "camera/camera.h"
#include "pipeline/pipelineBuilder.h"
#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/bufferSync.h"
#include "pipelineBuffer/gpuBuffer.h"
#include "renderPass/renderPass.h"
#include "shaders/loadShader.h"
#include <glad/glad.h>

Pipeline::Pipeline(const BuildPipeline& buildPipeline)
{
	setupCanvas();

	PipelineBuilder(*this).createPipeline(buildPipeline);
}

void Pipeline::render()
{

	for (const PipelineStage& stage : m_stages) {
		for (BufferSync* bufferSync : stage.m_bufferSyncs) {
			bufferSync->sync();
		}
		glFinish();

		for (const PipelinePass& pass : stage.m_passes) {
			pass.m_renderPass->render();
		}
		glFinish();
	}
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
