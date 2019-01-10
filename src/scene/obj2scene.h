#pragma once

#include "common.h"
#include <tiny_obj_loader.h>

class Scene;
class Mesh;
class ObjMaterial;
class Texture;

class Obj2scene {
public:
	static bool LoadScene(Scene* scene, const char* filename);

private:
	// helpers
	static void CalculateCenterAndBounds(Mesh* mesh);
	static void CreateMeshLight(Scene* scene, Mesh* mesh);

	static Texture* CreateTexture(Scene* scene, std::string filename, std::map<const std::string, Texture*>* textures);

	static void PrintInfo(const tinyobj::attrib_t& attrib,
						  const std::vector<tinyobj::shape_t>& shapes,
						  const std::vector<tinyobj::material_t>& materials);
};