#pragma once

// #include <math/vec3.h>
// #include <math/vec3fa.h>
// #include <math/affinespace.h>
#include <glm/glm.hpp>

#include "spectrum/rgbSpectrum.h"
// #include "sampler/stratifiedSampler.h"

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