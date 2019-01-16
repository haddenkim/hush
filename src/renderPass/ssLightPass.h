#pragma once
#include "renderPass/glPass.h"

class Scene;
class Camera;
class Buffer;

class SsLightPass : public GlPass {
public:
	SsLightPass(Scene* scene,
				Camera* camera,
				Buffer* positionBuffer,
				Buffer* normalBuffer,
				Buffer* matDiffuseBuffer,
				Buffer* matSpecularBuffer,
				Buffer* colorBuffer,
				GLuint canvasVAO);

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