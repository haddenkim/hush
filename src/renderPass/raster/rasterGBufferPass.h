#pragma once
#include "renderPass/glPass.h"

class Scene;
class Camera;
class Pipeline;
class GpuBuffer;

class RasterGBufferPass : public GlPass {
public:
	RasterGBufferPass(Scene* scene,
					  Camera* camera,

					  GpuBuffer* positionBuffer,
					  GpuBuffer* normalBuffer,
					  GpuBuffer* matAmbientBuffer,
					  GpuBuffer* matDiffuseBuffer,
					  GpuBuffer* matSpecularBuffer
	);

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
	const Scene* m_scene;
	const Camera* m_camera;

	size_t m_numMeshes;
	float* m_numMeshFaces;
	GLuint* m_VAOs;
	GLuint* m_texDiffuse; // model material texture

	// outputs
	GpuBuffer* m_positionBuffer;
	GpuBuffer* m_normalBuffer;
	GpuBuffer* m_matAmbientBuffer;
	GpuBuffer* m_matDiffuseBuffer;
	GpuBuffer* m_matSpecularBuffer;
};