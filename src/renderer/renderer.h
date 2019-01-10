#pragma once

#include "common.h"
#include <glad/glad.h>
#include <string>
#include <vector>

class Scene;
class Camera;

class Renderer {
public:
	Renderer(Scene* scene, Camera* camera);

	// main render functions
	virtual void render3d() = 0;
	virtual void renderPostProcess() = 0;
	void renderTonemap();
	void renderFramebuffer();

	// opengl
	GLuint m_displayTexPass;
	GLuint m_tonemapPass;
	GLuint m_canvasVAO;
	GLuint m_renderedFBO;

	GLuint m_preTonemapImage;
	GLuint m_postTonemapImage;

	// renderer settings
	uint m_width;
	uint m_height;

	// output settings
	int m_displayFramebuffer = 0;				 // index in m_framebufferTextures
	std::vector<GLuint> m_framebufferTextures;   // gl tex ids
	std::vector<std::string> m_framebufferNames; // for gui
	std::vector<bool> m_framebufferAllowed;		 // for gui

	// setup helper
	GLuint setupOutputTexture(GLenum format = GL_RGB);

protected:
	Scene* m_scene;
	Camera* m_camera;

	// setup helper
	void setupShader();
	void setupFBO();
	void setupCanvas();
	void addFrameBufferTexture(GLuint glId, std::string name);
};
