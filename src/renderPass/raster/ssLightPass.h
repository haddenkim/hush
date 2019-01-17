#pragma once
#include "renderPass/glPass.h"

class Scene;
class Camera;
class Pipeline;
class Buffer;

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
	Scene* m_scene;
	Camera* m_camera;
	Buffer* m_positionBuffer;
	Buffer* m_normalBuffer;
	Buffer* m_matDiffuseBuffer;
	Buffer* m_matSpecularBuffer;

	// data
	GLuint m_canvasVAO;

	// outputs
	Buffer* m_colorBuffer;
};