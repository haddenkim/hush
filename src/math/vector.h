#include "common.h"

inline Vec3f local2World(const Vec3f& localDir, const Vec3f& normal)
{
	// deterine the 3x3 transform matrix about z = normal
	Vec3f Nt, Nb;
	if (std::fabs(normal.x) > std::fabs(normal.y))
		Nt = glm::normalize(Vec3f(normal.z, 0, -normal.x));
	else
		Nt = glm::normalize(Vec3f(0, -normal.z, normal.y));
	Nb = glm::cross(normal, Nt);

	// convert direction to world space
	return glm::normalize(glm::mat3(Nt, Nb, normal) * localDir);
};

inline float AbsDot(const Vec3f& w0, const Vec3f& w1)
{
	return glm::abs(glm::dot(w0, w1));
}

template <typename T>
inline T Barycentric(const T& a, const T& b, const T& c, float u, float v)
{
	return (1.f - u - v) * a + u * b + v * c;
}