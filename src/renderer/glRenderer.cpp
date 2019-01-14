#include "glRenderer.h"

#include "camera/camera.h"
#include "light/light.h"
#include "material/material.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "shaders/loadShader.h"
#include "texture/texture.h"

GlRenderer::GlRenderer(Scene* scene, Camera* camera)
	: Renderer(scene, camera)
{
	setupShader();
	setupVAOs();
	setupMeshTextures();
	setupGeoFBO();
}

void GlRenderer::render3d()
{
	// clear
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	/* geometry pass */
	// use geo FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_geoFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use geo program
	glUseProgram(m_geometryPass);
	glEnable(GL_DEPTH_TEST);

	// load uniforms
	glUniformMatrix4fv(glGetUniformLocation(m_geometryPass, "VP"), 1, GL_FALSE, &m_camera->m_viewProjection[0][0]);

	// draw each mesh
	for (size_t i = 0; i < m_numMeshes; i++) {
		auto mesh = m_scene->m_meshList[i];
		if (!mesh->m_isEnabled) {
			continue;
		}

		glBindVertexArray(m_VAOs[i]);

		// material uniforms
		GlMaterial material = mesh->m_material->getGL();

		glUniform3fv(glGetUniformLocation(m_geometryPass, "material.diffuse"), 1, &material.m_diffuse[0]);
		glUniform3fv(glGetUniformLocation(m_geometryPass, "material.specular"), 1, &material.m_specular[0]);
		glUniform3fv(glGetUniformLocation(m_geometryPass, "material.ambient"), 1, &material.m_ambient[0]);
		glUniform3fv(glGetUniformLocation(m_geometryPass, "material.emission"), 1, &material.m_emission[0]);
		glUniform1f(glGetUniformLocation(m_geometryPass, "material.shininess"), material.m_shininess);

		// diffuse texture
		if (material.m_diffuseMap) {
			glUniform1i(glGetUniformLocation(m_geometryPass, "material.hasTexDiffuse"), 1); // true
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texDiffuse[i]);
		} else {
			glUniform1i(glGetUniformLocation(m_geometryPass, "material.hasTexDiffuse"), 0); // false
		}

		glDrawElements(GL_TRIANGLES, m_numMeshFaces[i] * 3, GL_UNSIGNED_INT, 0);
	}

	/* lighting pass  */
	// switch to renderer's final framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_renderedFBO);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// blending
	// CODEHERE - accurate blending function
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindVertexArray(m_canvasVAO);

	// use lighting program
	glUseProgram(m_lightingPass);

	// use geo pass's outputs
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gBufferTex[0]); // specular
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_gBufferTex[1]); // diffuse
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_gBufferTex[2]); // position
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_gBufferTex[3]); // normal

	// for each enabled light
	for (auto light : m_scene->m_enabledLightList) {

		// treat every light source as point light
		Spectrum intensity = light->m_power * ONE_OVER_PI * 0.25f;

		// pass to shader uniform
		glUniform3fv(glGetUniformLocation(m_lightingPass, "cameraPosition"), 1, &m_camera->m_position[0]);
		glUniform3fv(glGetUniformLocation(m_lightingPass, "light.position"), 1, &light->m_position[0]);
		glUniform3fv(glGetUniformLocation(m_lightingPass, "light.intensity"), 1, &intensity[0]);

		// draw lighting pass
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// CODEHERE - MESH LIGHTING SHADER, instead of above treating as point light above
	// for each mesh light

	/* ambient pass */
	if (m_scene->m_enableAmbientLight) {
		glUseProgram(m_ambientPass);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_gBufferTex[4]); // ambient
		glUniform1f(glGetUniformLocation(m_ambientPass, "ambientIntensity"), m_scene->m_ambientIntensity);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// unbind, reset
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GlRenderer::renderPostProcess()
{
	renderTonemap();
}

void GlRenderer::setupShader()
{
	/* geometry pass */
	m_geometryPass = LoadShader::createGlProgram("rasterGBuffer.vs",
												 "rasterGBuffer.fs");
	// uniforms
	glUniform1i(glGetUniformLocation(m_geometryPass, "material.texDiffuse"), 0); // diffuse texture

	/* lighting pass */
	m_lightingPass = LoadShader::createGlProgram("drawTexture.vs",
												 "rasterLight.fs");
	// uniforms
	glUseProgram(m_lightingPass);
	glUniform1i(glGetUniformLocation(m_lightingPass, "gSpecular"), 0);
	glUniform1i(glGetUniformLocation(m_lightingPass, "gDiffuse"), 1);
	glUniform1i(glGetUniformLocation(m_lightingPass, "gPosition"), 2);
	glUniform1i(glGetUniformLocation(m_lightingPass, "gNormal"), 3);

	/* ambient pass */
	m_ambientPass = LoadShader::createGlProgram("drawTexture.vs",
												"rasterAmbient.fs");
	glUseProgram(m_ambientPass);
	glUniform1i(glGetUniformLocation(m_ambientPass, "gAmbient"), 0);

	// unbind
	glUseProgram(0);
}

void GlRenderer::setupVAOs()
{
	m_numMeshes = m_scene->m_meshList.size();
	m_numMeshFaces = new float[m_numMeshes]; // saved for draw triangle count

	// VAO, VBO, EBO
	m_VAOs = new GLuint[m_numMeshes];
	GLuint vertexVBOs[m_numMeshes];
	GLuint normalVBOs[m_numMeshes];
	GLuint texcoordVBOs[m_numMeshes];
	GLuint facesEBOs[m_numMeshes];

	glGenVertexArrays(m_numMeshes, m_VAOs);
	glGenBuffers(m_numMeshes, vertexVBOs);
	glGenBuffers(m_numMeshes, normalVBOs);
	glGenBuffers(m_numMeshes, texcoordVBOs);
	glGenBuffers(m_numMeshes, facesEBOs);

	// setup buffers for each mesh
	for (size_t i = 0; i < m_numMeshes; i++) {
		auto mesh = m_scene->m_meshList[i];
		m_numMeshFaces[i] = mesh->m_faces.size();

		glBindVertexArray(m_VAOs[i]);

		// load vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(float) * 3, &mesh->m_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// load normals
		glBindBuffer(GL_ARRAY_BUFFER, normalVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_normals.size() * sizeof(float) * 3, &mesh->m_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		// tex coords
		glBindBuffer(GL_ARRAY_BUFFER, texcoordVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_texcoords.size() * sizeof(float) * 2, &mesh->m_texcoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);

		// load faces
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesEBOs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_faces.size() * sizeof(unsigned int) * 3, &mesh->m_faces[0], GL_STATIC_DRAW);
	}

	// unbind
	glBindVertexArray(0);
}

void GlRenderer::setupMeshTextures()
{
	m_numMeshes = m_scene->m_meshList.size();

	m_texDiffuse = new GLuint[m_numMeshes];
	glGenTextures(m_numMeshes, m_texDiffuse);

	// for each mesh
	for (size_t i = 0; i < m_numMeshes; i++) {
		GlMaterial material = m_scene->m_meshList[i]->m_material->getGL();

		// diffuse
		Texture* diffuseTexture = material.m_diffuseMap;
		if (diffuseTexture) {
			glBindTexture(GL_TEXTURE_2D, m_texDiffuse[i]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuseTexture->m_width, diffuseTexture->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuseTexture->m_data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GlRenderer::setupGeoFBO()
{
// specular, diffuse, position, normal, ambient
#define gBufferCount 5 // using a macro because clang complained 'variable-sized object may not be initialized'
	// int gBufferCount = 5;
	m_gBufferTex = new GLuint[gBufferCount];

	m_gBufferTex[0] = setupOutputTexture(GL_RGBA); // specular + shininess
	m_gBufferTex[1] = setupOutputTexture();		   // diffuse
	m_gBufferTex[2] = setupOutputTexture();		   // position
	m_gBufferTex[3] = setupOutputTexture();		   // normal
	m_gBufferTex[4] = setupOutputTexture();		   // ambient

	addFrameBufferTexture(m_gBufferTex[0], "Material - Specular");
	addFrameBufferTexture(m_gBufferTex[1], "Material - Diffuse");
	addFrameBufferTexture(m_gBufferTex[2], "Positions");
	addFrameBufferTexture(m_gBufferTex[3], "Normals");
	addFrameBufferTexture(m_gBufferTex[4], "Material - Ambient");

	glGenFramebuffers(1, &m_geoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_geoFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferTex[0], 0); // specular + shininess
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferTex[1], 0); // diffuse
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBufferTex[2], 0); // position
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBufferTex[3], 0); // normal
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_gBufferTex[4], 0); // ambient

	GLuint attachments[gBufferCount] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

	glDrawBuffers(gBufferCount, attachments);

	// depth RBO
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// restore defaults
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
