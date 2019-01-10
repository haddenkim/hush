#include "sampler.h"
#include <cassert>
#include <cmath>

Sampler::Sampler(int x, int y, int s)
{
}

float Sampler::get1D()
{
	return ((float) rand() / (RAND_MAX));
}

Vec2f Sampler::get2D()
{
	return Vec2f(get1D(), get1D());
}

int Sampler::getOneOf(int n)
{
	assert(n > 0);
	return std::min((int)(get1D() * n), n - 1);
}