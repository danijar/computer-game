#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageTransform
{
	vec3 Position, Rotation, Scale;
	mat4 Matrix;
	StorageTransform() : Position(0), Rotation(0), Scale(1) {}
};