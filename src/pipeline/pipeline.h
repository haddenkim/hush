#pragma once
#include "common.h"
#include "gui/iGuiEditable.h"
#include "pipeline/pipelineIO.h"
#include "pipelineBuffer/bufferManager.h"
#include "renderPass/renderPass.h"
#include <glad/glad.h>
#include <initializer_list>
#include <vector>

class Scene;
class Camera;
class Buffer;

class Pipeline : public IGuiEditable {
public:
	Pipeline(Scene* scene, Camera* camera, std::initializer_list<RenderPassType> passes);

	/* rendering */
	void render();

	/* Stages, Passes */
	void addPass(RenderPassType type, int position = -1);
	std::vector<RenderPass*> m_passes;

	/* Buffers */
	BufferManager m_bufferManager;

	/* draw to screen */
	uint m_displayedBufferIndex;
	GLuint m_canvasVAO;

	// UI
	bool guiEdit() override;
	void guiFramebuffer();

	// state
	const uint m_width;
	const uint m_height;
	const Scene* m_scene;
	const Camera* m_camera;


protected:
	// setup helpers
	void setupCanvas();

};
