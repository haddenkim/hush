#pragma once

#include "common.h"

class Sampler {
public:
	Sampler(int x, int y, int s);

	float get1D();
	Vec2f get2D();

	int getOneOf(int n);

};