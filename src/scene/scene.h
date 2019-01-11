#pragma once

#include "common.h"
#include "gui/iGuiEditable.h"
#include "gui/iGuiReadable.h"
#include <embree3/rtcore.h>
#include <vector>

class Mesh;
class Material;
class Light;
class MeshLight;
class Texture;

class Scene : public IGuiReadable, IGuiEditable {
public:
	~Scene();

	// UI
	void guiRead() const override;
	bool guiEdit() override;
	void disableAllMesh();
	void enableAllMesh();
	void disableMesh(uint id, bool updateList = true);
	void enableMesh(uint id, bool updateList = true);
	void updateActiveMeshes();
	void updateActiveLights();

	// Spectrum m_backgroundColor; // = glm::vec3(0.53f, 0.81f, 0.92f); // sky blue
	bool m_enableAmbientLight;
	float m_ambientIntensity;

	// data
	std::vector<Mesh*> m_meshList;
	std::vector<Mesh*> m_enabledMeshList;
	std::vector<Material*> m_materialList;
	std::vector<Light*> m_lightList;
	std::vector<Light*> m_enabledLightList;
	std::vector<Texture*> m_textureList;

	// embree
	RTCDevice m_embDevice;
	RTCScene m_embScene;
	std::vector<uint> m_embGeoIds;

	// helpers
	void setupAdditionalLights();
	void setupEmb();
	void loadEmbScene();
	void loadEmbGeometry(Mesh* pMesh);

	// properties
	Point3f m_center;
	Point3f m_minBounds;
	Point3f m_maxBounds;

	// stats
	int m_vertexCount = 0;
	int m_triCount = 0;
	int m_emissiveMeshCount = 0;
};