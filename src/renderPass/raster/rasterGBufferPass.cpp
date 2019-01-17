#include "rasterGBufferPass.h"
#include "camera/camera.h"
#include "material/material.h"
#include "pipeline/pipeline.h"
#include "pipelineBuffer/buffer.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "shaders/loadShader.h"
#include "texture/texture.h"

RasterGBufferPass::RasterGBufferPass(Pipeline* pipeline)
	: GlPass("Raster G-Buffer",
			 RASTER_GBUFFER,
			 { CAM_VP, MESH, MAT_AMBIENT, MAT_DIFFUSE, MAT_SPECULAR },				 // inputs
			 { G_POSITION, G_NORMAL, G_MAT_AMBIENT, G_MAT_DIFFUSE, G_MAT_SPECULAR }) // outputs
	, m_scene(pipeline->m_scene)
	, m_camera(pipeline->m_camera)
	, m_positionBuffer(pipeline->getOrCreateBuffer(G_POSITION))
	, m_normalBuffer(pipeline->getOrCreateBuffer(G_NORMAL))
	, m_matAmbientBuffer(pipeline->getOrCreateBuffer(G_MAT_AMBIENT))
	, m_matDiffuseBuffer(pipeline->getOrCreateBuffer(G_MAT_DIFFUSE))
	, m_matSpecularBuffer(pipeline->getOrCreateBuffer(G_MAT_SPECULAR))
{
	setupShader();
	setupFBO();
	setupVAOs();
	setupMeshTextures();
}

void RasterGBufferPass::render()
{
	// use geo FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use geo program
	glUseProgram(m_shader);
	glEnable(GL_DEPTH_TEST);

	// load uniforms
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "VP"), 1, GL_FALSE, &m_camera->m_viewProjection[0][0]);

	// draw each mesh
	for (size_t i = 0; i < m_numMeshes; i++) {
		auto mesh = m_scene->m_meshList[i];
		if (!mesh->m_isEnabled) {
			continue;
		}

		glBindVertexArray(m_VAOs[i]);

		// material uniforms
		GlMaterial material = mesh->m_material->getGL();

		glUniform3fv(glGetUniformLocation(m_shader, "material.ambient"), 1, &material.m_ambient[0]);
		glUniform3fv(glGetUniformLocation(m_shader, "material.diffuse"), 1, &material.m_diffuse[0]);
		glUniform3fv(glGetUniformLocation(m_shader, "material.specular"), 1, &material.m_specular[0]);
		glUniform1f(glGetUniformLocation(m_shader, "material.shininess"), material.m_shininess);
		// glUniform3fv(glGetUniformLocation(m_shader, "material.emission"), 1, &material.m_emission[0]);

		// diffuse texture
		if (material.m_diffuseMap) {
			glUniform1i(glGetUniformLocation(m_shader, "material.hasTexDiffuse"), 1); // true
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texDiffuse[i]);
		} else {
			glUniform1i(glGetUniformLocation(m_shader, "material.hasTexDiffuse"), 0); // false
		}

		glDrawElements(GL_TRIANGLES, m_numMeshFaces[i] * 3, GL_UNSIGNED_INT, 0);
	}

	// unbind, reset
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// setup helpers
void RasterGBufferPass::setupShader()
{
	m_shader = LoadShader::createGlProgram("rasterGBuffer.vs",
										   "rasterGBuffer.fs");

	// uniforms
	glUseProgram(m_shader);
	glUniform1i(glGetUniformLocation(m_shader, "gPosition"), 0);
	glUniform1i(glGetUniformLocation(m_shader, "gNormal"), 1);
	glUniform1i(glGetUniformLocation(m_shader, "gAmbient"), 2);
	glUniform1i(glGetUniformLocation(m_shader, "gDiffuse"), 3);
	glUniform1i(glGetUniformLocation(m_shader, "gSpecular"), 4);

	// unbind
	glUseProgram(0);
}

void RasterGBufferPass::setupFBO()
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionBuffer->m_glId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalBuffer->m_glId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_matAmbientBuffer->m_glId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_matDiffuseBuffer->m_glId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_matSpecularBuffer->m_glId, 0);

	GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// depth RBO
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_camera->m_width, m_camera->m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", status);
	}

	// unbind
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RasterGBufferPass::setupVAOs()
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

void RasterGBufferPass::setupMeshTextures()
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