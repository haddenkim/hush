#pragma once

#include "common.h"
#include <tiny_obj_loader.h>

class Scene;
class Mesh;
class Material;
class Texture;

class Obj2scene {
public:
	static bool LoadScene(Scene* scene, const char* filename);

private:
	// helpers
	static void CalculateCenterAndBounds(Mesh* mesh);
	static void CreateMeshLight(Scene* scene, Mesh* mesh);

	static void CreateMaterial(Scene* scene, const tinyobj::material_t& tinyMat, std::string directory, std::map<const std::string, Texture*>* textures);
	static Texture* CreateTexture(Scene* scene, std::string directory, std::string filename, std::map<const std::string, Texture*>* textures);
};