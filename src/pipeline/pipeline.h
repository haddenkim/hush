#pragma once
#include "common.h"
#include "gui/iGuiEditable.h"
#include "pipeline/pipelineIO.h"
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

	void render();
	void drawToScreen();
	void selectDrawBuffer(uint index);

	// modification
	void addPass(RenderPassType type, int position = -1);
	Buffer* getOrCreateBuffer(PipelineIO type, bool isCPU = false);

	std::vector<RenderPass*> m_passes;

	std::vector<Buffer*> m_buffers;
	std::vector<PipelineIO> m_bufferTypes;
	uint m_displayedBufferIndex;

	// renderer settings
	uint m_width;
	uint m_height;

	// opengl
	GLuint m_displayColorShader;
	GLuint m_canvasVAO;

	Scene* m_scene;
	Camera* m_camera;

	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader();
	void setupCanvas();
};
