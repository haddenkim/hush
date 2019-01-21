#pragma once
#include "common.h"
#include "gui/iGuiEditable.h"
// #include "pipeline/pipelineBuilder.h"
#include "pipeline/pipelineIO.h"

#include "renderPass/renderPass.h"
#include <glad/glad.h>
#include <initializer_list>
#include <vector>

class Scene;
class Camera;
class Buffer;
class BufferSync;
struct BuildPipeline;

struct PipelineIOLink {
	uint m_fromStage;
	uint m_fromPass;
	// uint toStage;
	// uint toPass;
	std::vector<PipelineIO> m_types;
};

struct PipelinePass {
	RenderPass* m_renderPass;
	std::vector<Buffer*> m_buffers;
	std::vector<PipelineIOLink> m_links;
};

struct PipelineStage {
	std::vector<BufferSync*> m_bufferSyncs;

	std::vector<PipelinePass> m_passes;
};

class Pipeline : public IGuiEditable {
public:
	Pipeline(const BuildPipeline& buildPipeline);

	/* rendering */
	void render();

	/* draw to screen */
	Buffer* m_displayBuffer;
	GLuint m_canvasVAO;

	// UI
	bool guiEdit() override;
	void guiFramebuffer();

	// state
	uint m_width;
	uint m_height;
	Scene* m_scene;
	Camera* m_camera;
	std::vector<PipelineStage> m_stages;

protected:
	// setup helpers
	void setupCanvas();
};
