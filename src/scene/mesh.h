#pragma once

#include "common.h"
#include <string>
#include <vector>

struct Material;
class Light;

struct Mesh {

	std::string m_name;

	std::vector<Point3f> m_vertices;  // per vertex position
	std::vector<Vec3f> m_normals;	 // per vertex normal
	std::vector<Point2f> m_texcoords; // per vertex texcoords
	std::vector<Vec3i> m_faces;

	Material* m_material;
	Light* m_light;

	// UI
	bool m_isEnabled = true;

	// convienences
	float m_surfaceArea;
	float m_invSurfaceArea;
	Point3f m_center;
	Point3f m_minBounds;
	Point3f m_maxBounds;
};