#include "pipeline.h"
#include "camera/camera.h"
#include "pipelineBuffer/buffer.h"
#include "pipelineBuffer/gpuBuffer.h"

#include "renderPass/renderPass.h"
#include "shaders/loadShader.h"
#include <glad/glad.h>

Pipeline::Pipeline(Scene* scene, Camera* camera, std::initializer_list<RenderPassType> passes)
	: m_scene(scene)
	, m_camera(camera)
	, m_width(camera->m_width)
	, m_height(camera->m_height)
	, m_bufferManager(camera->m_width, camera->m_height)

{
	setupCanvas();

	for (RenderPassType passType : passes) {
		addPass(passType);
	}

	// initial settings
	m_displayedBufferIndex = m_bufferManager.size() - 1; // last buffer
}

void Pipeline::render()
{
	for (RenderPass* pass : m_passes) {
		pass->render();
	}
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
