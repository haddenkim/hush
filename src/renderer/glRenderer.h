#pragma once

#include "renderer/renderer.h"
#include <glad/glad.h>

class GlRenderer : public Renderer {
public:
	GlRenderer(Scene* scene, Camera* camera);
	void render3d() override;
	void renderPostProcess() override;

private:
	// shader programs
	GLuint m_geometryPass;
	GLuint m_lightingPass;
	GLuint m_ambientPass;

	// per mesh
	size_t m_numMeshes;
	float* m_numMeshFaces;
	GLuint* m_VAOs;
	GLuint* m_texDiffuse; // model material texture

	// framebuffer
	GLuint m_geoFBO;
	GLuint* m_gBufferTex;

	// helper functions
	void setupShader();
	void setupVAOs();
	void setupMeshTextures();
	void setupGeoFBO();
};
