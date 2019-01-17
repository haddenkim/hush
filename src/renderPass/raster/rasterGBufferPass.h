#pragma once
#include "renderPass/glPass.h"

class Scene;
class Camera;
class Pipeline;
class Buffer;

class RasterGBufferPass : public GlPass {
public:
	RasterGBufferPass(Pipeline* pipeline);

	void render() override;
	// UI
	bool guiEdit() override;

protected:
	// setup helpers
	void setupShader() override;
	void setupFBO() override;

	void setupVAOs();
	void setupMeshTextures();

	// inputs
	Scene* m_scene;
	Camera* m_camera;

	size_t m_numMeshes;
	float* m_numMeshFaces;
	GLuint* m_VAOs;
	GLuint* m_texDiffuse; // model material texture

	// outputs
	Buffer* m_positionBuffer;
	Buffer* m_normalBuffer;
	Buffer* m_matAmbientBuffer;
	Buffer* m_matDiffuseBuffer;
	Buffer* m_matSpecularBuffer;
};