// copied code from the sample usage code in https://github.com/syoyo/tinyobjloader
#include "obj2scene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "light/meshLight.h"
#include "material/material.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "texture/texture.h"
#include <iostream>
#include <set>

bool Obj2scene::LoadScene(Scene* scene, const char* filename)
{
	/* tiny obj data */
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	/* retrieve the obj file's base directory */
	std::string base_dir = ".";
	std::string filepath = filename;
	if (filepath.find_last_of("/\\") != std::string::npos) {
		base_dir = filepath.substr(0, filepath.find_last_of("/\\"));
	}
#ifdef _WIN32
	base_dir += "\\";
#else
	base_dir += "/";
#endif

	/* tiny obj parse file */
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, base_dir.c_str());

	if (!err.empty()) {
		printf("tinyobj err: %s", err.c_str());
	}
	if (!ret) {
		return false;
	}

	/* Materials */
	// map of texture names to textures for shared texture lookup
	std::map<const std::string, Texture*> textures;

	// create material for each tiny_obj material
	for (const auto& tinyMat : materials) {
		Material* material = new Material();

		material->m_name = tinyMat.name;

		material->m_ambient = Spectrum(tinyMat.ambient[0], tinyMat.ambient[1], tinyMat.ambient[2]);
		material->m_diffuse = Spectrum(tinyMat.diffuse[0], tinyMat.diffuse[1], tinyMat.diffuse[2]);
		material->m_specular = Spectrum(tinyMat.specular[0], tinyMat.specular[1], tinyMat.specular[2]);
		material->m_transmittance = Spectrum(tinyMat.transmittance[0], tinyMat.transmittance[1], tinyMat.transmittance[2]);
		material->m_emission = Spectrum(tinyMat.emission[0], tinyMat.emission[1], tinyMat.emission[2]);

		material->m_shininess = tinyMat.shininess;
		material->m_refractionIndex = tinyMat.ior;

		material->m_isDiffuse = !material->m_diffuse.isBlack();
		material->m_isSpecular = !material->m_specular.isBlack();
		material->m_isTransmissive = !material->m_transmittance.isBlack();
		material->m_isEmissive = !material->m_emission.isBlack();

		// textures
		if (tinyMat.diffuse_texname.length() > 0) {
			material->m_diffuseTexture = CreateTexture(scene, base_dir + tinyMat.diffuse_texname, &textures);
		}
		// CODEHERE - other textured material properties (specular, transmissive, bump map)

		scene->m_materialList.push_back(material);
	}

	// create a final default material, in case obj file does not provide any materials
	auto defMatIndex = scene->m_materialList.size();
	Material* defaultMaterial = new Material();
	defaultMaterial->m_ambient = Spectrum(0.5f, 0.5f, 0.5f);
	defaultMaterial->m_diffuse = Spectrum(0.5f, 0.5f, 0.5f);
	defaultMaterial->m_specular = Spectrum(0.3f, 0.3f, 0.3f);
	defaultMaterial->m_shininess = 5.f;
	defaultMaterial->m_isDiffuse = true;
	defaultMaterial->m_isSpecular = true;
	scene->m_materialList.push_back(defaultMaterial);

	/* Mesh , Shapes */
	for (const tinyobj::shape_t& shape : shapes) {

		/////// DEBUG
		// for (size_t i = 0; i < shapes.size(); i++) {
		// 	const tinyobj::shape_t& shape = shapes[i];

		// 	if (i != 1)
		// 		continue;
		// 	/////// DEBUG

		const tinyobj::mesh_t& tinyMesh = shape.mesh;

		// create unique set of mesh's material(s)
		std::set<int> materialIds;
		for (int matId : tinyMesh.material_ids) {
			materialIds.insert(matId);
		}

		// for each material in the set
		for (int matId : materialIds) {
			// create a new scene mesh per material
			Mesh* sceneMesh = new Mesh();

			// for each face in the mesh
			for (size_t f = 0; f < tinyMesh.num_face_vertices.size(); f++) {
				// skip faces which do not share the current material
				if (tinyMesh.material_ids[f] != matId) {
					continue;
				}

				// because of tiny obj triangulation, all faces should be tris
				assert(tinyMesh.num_face_vertices[f] == 3);

				// vertex attributes
				auto indexOffset = f * 3;
				auto idx0 = tinyMesh.indices[indexOffset + 0];
				auto idx1 = tinyMesh.indices[indexOffset + 1];
				auto idx2 = tinyMesh.indices[indexOffset + 2];

				/* vertex positions */
				Vec3f v0(attrib.vertices[3 * idx0.vertex_index + 0],
						 attrib.vertices[3 * idx0.vertex_index + 1],
						 attrib.vertices[3 * idx0.vertex_index + 2]);

				Vec3f v1(attrib.vertices[3 * idx1.vertex_index + 0],
						 attrib.vertices[3 * idx1.vertex_index + 1],
						 attrib.vertices[3 * idx1.vertex_index + 2]);

				Vec3f v2(attrib.vertices[3 * idx2.vertex_index + 0],
						 attrib.vertices[3 * idx2.vertex_index + 1],
						 attrib.vertices[3 * idx2.vertex_index + 2]);

				// add vertex positions to mesh list
				auto faceIndex = sceneMesh->m_vertices.size();
				sceneMesh->m_vertices.push_back(v0);
				sceneMesh->m_vertices.push_back(v1);
				sceneMesh->m_vertices.push_back(v2);

				/* surface area */
				auto e01 = v1 - v0;
				auto e02 = v2 - v0;
				sceneMesh->m_surfaceArea += 0.5f * glm::length(cross(e01, e02));

				/* vertex normals */
				// explicit > smoothing group > face normal
				Vec3f n0, n1, n2;
				bool hasExplicitNormals = false;
				bool hasSmoothingGroup = false;

				// check if obj file provides explicit normals
				if (idx0.normal_index >= 0 && idx1.normal_index >= 0 && idx2.normal_index >= 0) {
					hasExplicitNormals = true;
				}

				if (hasExplicitNormals) {
					n0 = Vec3f(attrib.normals[3 * idx0.normal_index + 0],
							   attrib.normals[3 * idx0.normal_index + 1],
							   attrib.normals[3 * idx0.normal_index + 2]);

					n1 = Vec3f(attrib.normals[3 * idx1.normal_index + 0],
							   attrib.normals[3 * idx1.normal_index + 1],
							   attrib.normals[3 * idx1.normal_index + 2]);

					n2 = Vec3f(attrib.normals[3 * idx2.normal_index + 0],
							   attrib.normals[3 * idx2.normal_index + 1],
							   attrib.normals[3 * idx2.normal_index + 2]);

					// } else if (hasSmoothingGroup) {
					// CODEHERE - compute smoothing group normals

				} else {
					// compute face normal
					n0 = n1 = n2 = normalize(cross(e01, e02));
				}

				// add vertex normals to mesh list
				sceneMesh->m_normals.push_back(n0);
				sceneMesh->m_normals.push_back(n1);
				sceneMesh->m_normals.push_back(n2);

				/* tex coords */
				Point2f t0, t1, t2;

				// check if obj file provides tex coords
				if (idx0.texcoord_index >= 0 && idx1.texcoord_index >= 0 && idx2.texcoord_index >= 0) {
					// add the 3 uv coords to the scene mesh
					t0 = Point2f(attrib.texcoords[2 * idx0.texcoord_index + 0],
								 attrib.texcoords[2 * idx0.texcoord_index + 1]);
					t1 = Point2f(attrib.texcoords[2 * idx1.texcoord_index + 0],
								 attrib.texcoords[2 * idx1.texcoord_index + 1]);
					t2 = Point2f(attrib.texcoords[2 * idx2.texcoord_index + 0],
								 attrib.texcoords[2 * idx2.texcoord_index + 1]);
				} else {
					// add uv coords of 0,0
					t0 = t1 = t2 = Point2f(0.f, 0.f);
				}

				sceneMesh->m_texcoords.push_back(t0);
				sceneMesh->m_texcoords.push_back(t1);
				sceneMesh->m_texcoords.push_back(t2);

				/* face indices */
				sceneMesh->m_faces.emplace_back(Vec3i(faceIndex, faceIndex + 1, faceIndex + 2));

				/* scene stats */
				scene->m_vertexCount += 3;
				scene->m_triCount++;
			}

			/* center and min, max bounds */
			CalculateCenterAndBounds(sceneMesh);

			/* inverse surface area */
			sceneMesh->m_invSurfaceArea = 1.f / sceneMesh->m_surfaceArea;

			/* mesh material */
			// if obj file does not have a material for this mesh, use default material
			if (matId == -1)
				matId = defMatIndex;

			sceneMesh->m_material = scene->m_materialList[matId];

			/* mesh name */
			sceneMesh->m_name = shape.name;
			// append material to differentiate submeshes
			if (materialIds.size() > 1) {
				sceneMesh->m_name += "|" + sceneMesh->m_material->m_name;
			}

			/* light */
			// create a mesh light if the mesh is emmisive
			if (sceneMesh->m_material->m_isEmissive) {
				CreateMeshLight(scene, sceneMesh);
			}

			/* add to scene */
			scene->m_meshList.push_back(sceneMesh);
		}
	}

	/* active lists */
	scene->updateActiveMeshes();
	scene->updateActiveLights();

	/* scene properties/stats */
	// compute scene bounds & center
	scene->m_minBounds = Point3f(HUGE_VALF, HUGE_VALF, HUGE_VALF);
	scene->m_maxBounds = Point3f(-HUGE_VALF, -HUGE_VALF, -HUGE_VALF);

	for (Mesh* mesh : scene->m_meshList) {
		if (mesh->m_minBounds.x < scene->m_minBounds.x)
			scene->m_minBounds.x = mesh->m_minBounds.x;
		if (mesh->m_minBounds.y < scene->m_minBounds.y)
			scene->m_minBounds.y = mesh->m_minBounds.y;
		if (mesh->m_minBounds.z < scene->m_minBounds.z)
			scene->m_minBounds.z = mesh->m_minBounds.z;

		if (mesh->m_maxBounds.x > scene->m_maxBounds.x)
			scene->m_maxBounds.x = mesh->m_maxBounds.x;
		if (mesh->m_maxBounds.y > scene->m_maxBounds.y)
			scene->m_maxBounds.y = mesh->m_maxBounds.y;
		if (mesh->m_maxBounds.z > scene->m_maxBounds.z)
			scene->m_maxBounds.z = mesh->m_maxBounds.z;
	}

	scene->m_center = (scene->m_minBounds + scene->m_maxBounds) * 0.5f;

	return true;
}

void Obj2scene::CalculateCenterAndBounds(Mesh* mesh)
{
	mesh->m_minBounds = Point3f(HUGE_VALF, HUGE_VALF, HUGE_VALF);
	mesh->m_maxBounds = Point3f(-HUGE_VALF, -HUGE_VALF, -HUGE_VALF);

	for (const Point3f& vert : mesh->m_vertices) {
		if (vert.x < mesh->m_minBounds.x)
			mesh->m_minBounds.x = vert.x;
		if (vert.y < mesh->m_minBounds.y)
			mesh->m_minBounds.y = vert.y;
		if (vert.z < mesh->m_minBounds.z)
			mesh->m_minBounds.z = vert.z;

		if (vert.x > mesh->m_maxBounds.x)
			mesh->m_maxBounds.x = vert.x;
		if (vert.y > mesh->m_maxBounds.y)
			mesh->m_maxBounds.y = vert.y;
		if (vert.z > mesh->m_maxBounds.z)
			mesh->m_maxBounds.z = vert.z;
	}

	mesh->m_center = (mesh->m_minBounds + mesh->m_maxBounds) * 0.5f;
}

Texture* Obj2scene::CreateTexture(Scene* scene, std::string filename, std::map<const std::string, Texture*>* textures)
{
#ifdef _WIN32
	//
#else
	std::replace(filename.begin(), filename.end(), '\\', '/');
#endif

	// find the texture in case it already was created
	auto search = textures->find(filename);
	if (search != textures->end()) {
		return search->second;
	} else {
		// if not found, create new texture
		Texture* texture = new Texture();

		// stbi setting to output data starting from bottom left, to match openGL standard
		stbi_set_flip_vertically_on_load(true);

		int w, h, n;
		unsigned char* image = stbi_load(filename.c_str(), &w, &h, &n, STBI_rgb);

		if (!image) {
			printf("STBI err - filename: %s\n", filename.c_str());
		}

		texture->m_width = w;
		texture->m_height = h;
		texture->m_data = image;

		// add texture to scene list
		scene->m_textureList.push_back(texture);

		// update map
		textures->insert(std::make_pair(filename, texture));
		return texture;
	}
}

void Obj2scene::CreateMeshLight(Scene* scene, Mesh* mesh)
{
	MeshLight* light = new MeshLight(mesh);

	mesh->m_light = light;

	scene->m_lightList.push_back(light);
	scene->m_emissiveMeshCount++;
}
