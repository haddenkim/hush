#pragma once

// #include <math/vec3.h>
// #include <math/vec3fa.h>
// #include <math/affinespace.h>
#include <glm/glm.hpp>

#include "spectrum/rgbSpectrum.h"
// #include "sampler/stratifiedSampler.h"

// constants
const float ONE_OVER_PI = 0.31830988618379069122f;  // 1/PI
const float ONE_OVER_2PI = 0.15915494309189534561f; // 1/(2PI)
const float PI = 3.14159265358979323846f;			// PI
const float TWO_PI = 6.283185307179586232f;			// 2PI

/* scalar types */
typedef unsigned int uint;

/* point types */
typedef glm::ivec2 Point2i;
typedef glm::vec2 Point2f;
typedef glm::vec3 Point3f;

/* vector types */
typedef glm::ivec2 Vec2i;
typedef glm::vec2 Vec2f;

typedef glm::ivec3 Vec3i;
typedef glm::vec3 Vec3f;
// typedef embree::Vec3fa Vec3fa;

/* matrix types */
// typedef embree::AffineSpace3f Mat4;
typedef glm::mat4 Mat4;

/* spectrum types */
typedef RGBSpectrum<float> Spectrum;

/* sampler */
// typedef StratifiedSampler AppSampler;