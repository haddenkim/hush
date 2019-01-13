#include "surfaceInteraction.h"

#include "material/material.h"
#include "math/vector.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include <cassert>
#include <embree3/rtcore.h>

SurfaceInteraction::SurfaceInteraction(const RTCRayHit& rayHit, const Scene& scene)
	: SurfaceInteraction(rayHit, *scene.m_meshList[rayHit.hit.geomID])
{
}

SurfaceInteraction::SurfaceInteraction(const RTCRayHit& rayHit, const Mesh& mesh)
{
	uint primID = rayHit.hit.primID;
	assert(primID < mesh.m_faces.size());
	Vec3i face = mesh.m_faces[primID];

	m_wo = -glm::normalize(Vec3f(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));
	m_position = Vec3f(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z) - m_wo * rayHit.ray.tfar;
	m_normalGeo = glm::normalize(Vec3f(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));

	m_normalShade = Barycentric(mesh.m_normals[face.x], mesh.m_normals[face.y], mesh.m_normals[face.z], rayHit.hit.u, rayHit.hit.v);
	m_texCoord = Barycentric(mesh.m_texcoords[face.x], mesh.m_texcoords[face.y], mesh.m_texcoords[face.z], rayHit.hit.u, rayHit.hit.v);

	m_material = mesh.m_material;
	m_light = mesh.m_light;

	m_diffuse = m_material->getDiffuse(m_texCoord);
	m_specular = m_material->getSpecular(m_texCoord);
	m_shininess = m_material->getShininess(m_texCoord);
}
