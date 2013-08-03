#pragma once

#include <GLM/glm.hpp>
using namespace glm;


struct StorageMovement
{
	StorageMovement() : Type(RANDOM) {}
	enum Type { RANDOM, PLANAR } Type;
	vec3 Direction;
};
