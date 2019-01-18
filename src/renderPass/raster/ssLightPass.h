#pragma once
#include "renderPass/glPass.h"

class Scene;
class Camera;
class Pipeline;
class GpuBuffer;

class SsLightPass : public GlPass {
public:
	SsLightPass(Pipeline* pipeline);

	void render() override;
	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader() override;
	void setupFBO() override;

	// inputs
	const Scene* m_scene;
	const Camera* m_camera;
	GpuBuffer* m_positionBuffer;
	GpuBuffer* m_normalBuffer;
	GpuBuffer* m_matDiffuseBuffer;
	GpuBuffer* m_matSpecularBuffer;

	// data
	GLuint m_canvasVAO;

	// outputs
	GpuBuffer* m_colorBuffer;
};