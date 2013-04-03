#pragma once

#include <GLM/glm.hpp>
using namespace glm;


struct StorageTransform
{
	StorageTransform() : Position(0), Rotation(0), Scale(1), Changed(true) {}
	vec3 Position, Rotation, Scale;
	mat4 Matrix;
	bool Static;
	bool Changed;
};
