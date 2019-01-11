#include "scene.h"
#include "light/light.h"
#include "light/pointLight.h"
#include "material/material.h"
#include "scene/mesh.h"
#include "texture/texture.h"
#include <cassert>

Scene::~Scene()
{
	for (auto mesh : m_meshList)
		delete (mesh);

	for (auto light : m_lightList)
		delete (light);

	for (auto mat : m_materialList)
		delete (mat);

	for (auto tex : m_textureList)
		delete (tex);

	rtcReleaseScene(m_embScene);
	rtcReleaseDevice(m_embDevice);
}

void Scene::disableAllMesh()
{
	for (uint i = 0; i < m_meshList.size(); i++) {
		disableMesh(i, false);
	}
	updateActiveMeshes();
}

void Scene::enableAllMesh()
{
	for (uint i = 0; i < m_meshList.size(); i++) {
		enableMesh(i, false);
	}
	updateActiveMeshes();
}

void Scene::disableMesh(uint id, bool updateList)
{
	assert(id < m_meshList.size());

	// disable mesh for openGL and GUI
	m_meshList[id]->m_isEnabled = false;

	// disable mesh for Embree
	uint embGeoId = m_embGeoIds[id];
	RTCGeometry embGeo = rtcGetGeometry(m_embScene, embGeoId);
	rtcDisableGeometry(embGeo);
	rtcCommitScene(m_embScene);

	// update list
	if (updateList) {
		updateActiveMeshes();
	}
}

void Scene::enableMesh(uint id, bool updateList)
{
	assert(id < m_meshList.size());

	// enable mesh for openGL and GUI
	m_meshList[id]->m_isEnabled = true;

	// enable mesh for Embree
	uint embGeoId = m_embGeoIds[id];
	RTCGeometry embGeo = rtcGetGeometry(m_embScene, embGeoId);
	rtcEnableGeometry(embGeo);
	rtcCommitScene(m_embScene);

	// update list
	if (updateList) {
		updateActiveMeshes();
	}
}

void Scene::updateActiveMeshes()
{
	m_enabledMeshList.clear();
	for (Mesh* mesh : m_meshList) {
		if (mesh->m_isEnabled) {
			m_enabledMeshList.push_back(mesh);
		}
	}
}

void Scene::updateActiveLights()
{
	m_enabledLightList.clear();
	for (Light* light : m_lightList) {
		if (light->m_isEnabled) {
			m_enabledLightList.push_back(light);
		}
	}
}

void Scene::setupAdditionalLights()
{
	PointLight* lightA = new PointLight("Point Light +xyz",					// name
										m_maxBounds + Vec3f(2.f, 2.f, 2.f), // position
										Spectrum(5.f, 5.f, 5.f));			// intensity
	lightA->m_isEnabled = false;

	PointLight* lightB = new PointLight("Point Light -xyz",					// name
										m_minBounds - Vec3f(2.f, 2.f, 2.f), // position
										Spectrum(5.f, 5.f, 5.f));			// intensity
	lightB->m_isEnabled = false;

	PointLight* lightC = new PointLight("Point Light center",	 // name
										m_center,				  // position
										Spectrum(5.f, 5.f, 5.f)); // intensity
	lightC->m_isEnabled = false;

	m_lightList.push_back(lightA);
	m_lightList.push_back(lightB);
	m_lightList.push_back(lightC);
}

void Scene::setupEmb()
{
	/* Embree docs strongly recommend below MXCSR settings */
	/* for best performance set FTZ and DAZ flags in MXCSR control and status register */
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	/* create new Embree device */
	m_embDevice = rtcNewDevice("hugepages=1");

	/* create embree scene */
	m_embScene = rtcNewScene(m_embDevice);

	loadEmbScene(); // load embree scene with Scene data

	rtcSetSceneBuildQuality(m_embScene, RTC_BUILD_QUALITY_HIGH);
	rtcCommitScene(m_embScene);
}

void Scene::loadEmbScene()
{
	for (auto mesh : m_meshList) {
		loadEmbGeometry(mesh);
	}
}

void Scene::loadEmbGeometry(Mesh* pMesh)
{
	struct Vertex {
		float x, y, z, r;
	};

	auto vertexCount = pMesh->m_vertices.size();
	auto triCount = pMesh->m_faces.size();

	RTCGeometry embMesh = rtcNewGeometry(m_embDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

	// buffer vertex positions
	Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(embMesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), vertexCount);
	for (size_t v = 0; v < vertexCount; v++) {
		vertices[v].x = pMesh->m_vertices[v].x;
		vertices[v].y = pMesh->m_vertices[v].y;
		vertices[v].z = pMesh->m_vertices[v].z;
	}

	// buffer normals
	// CODEHERE - consider buffering normals and additional data (texcoords)

	// buffer triangle indices
	unsigned* triangles = (unsigned*)rtcSetNewGeometryBuffer(embMesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(unsigned) * 3, triCount);
	for (size_t t = 0; t < triCount; t++) {
		triangles[t * 3] = pMesh->m_faces[t].x;
		triangles[t * 3 + 1] = pMesh->m_faces[t].y;
		triangles[t * 3 + 2] = pMesh->m_faces[t].z;
	}

	rtcCommitGeometry(embMesh);
	uint geomID = rtcAttachGeometry(m_embScene, embMesh);
	m_embGeoIds.push_back(geomID);
	rtcReleaseGeometry(embMesh);
}