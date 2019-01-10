#include "meshLight.h"
#include "light/lightSample.h"
#include "material/material.h"
#include "math/vector.h"
#include "sampler/sampler.h"
#include "scene/mesh.h"

MeshLight::MeshLight(Mesh* mesh)
	: Light(mesh->m_name,
			mesh->m_center,
			mesh->m_material->m_emission * mesh->m_surfaceArea * M_PI)
	, m_mesh(mesh)
	, m_radiance(mesh->m_material->m_emission)
{
	m_isDelta = false;
}

LightSample MeshLight::sampleLi(const Point3f surfacePosition, Sampler& sampler) const
{

	// select random tri on mesh
	int numFaces = m_mesh->m_faces.size();
	const Vec3i& selectedFace = m_mesh->m_faces[sampler.getOneOf(numFaces)];

	// random barycentric coord on that tri
	Point2f r = sampler.get2D();
	float sqrtR0 = std::sqrt(r.x);
	float u = 1.f - sqrtR0;
	float v = r.y * sqrtR0;

	// mesh point in world space
	const Point3f& p0 = m_mesh->m_vertices[selectedFace.x];
	const Point3f& p1 = m_mesh->m_vertices[selectedFace.y];
	const Point3f& p2 = m_mesh->m_vertices[selectedFace.z];
	Vec3f point = Barycentric(p0, p1, p2, u, v);

	// mesh normal in world space
	const Vec3f& n0 = m_mesh->m_normals[selectedFace.x];
	const Vec3f& n1 = m_mesh->m_normals[selectedFace.y];
	const Vec3f& n2 = m_mesh->m_normals[selectedFace.z];
	Vec3f normal = Barycentric(n0, n1, n2, u, v);

	// sample results
	LightSample ret;
	ret.m_direction = glm::normalize(point - surfacePosition);
	ret.m_distance = glm::distance(point, surfacePosition);

	float cosD = glm::dot(-normal, ret.m_direction);

	ret.m_pdf = (ret.m_distance * ret.m_distance) * m_mesh->m_invSurfaceArea / cosD;
	ret.m_radiance = cosD > 0.f ? m_radiance : Spectrum(0.f);

	return ret;
}

Spectrum MeshLight::le() const
{
	return m_radiance;
}