#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageTransform : public Storage
{
	StorageTransform() : Angle(0) {}
	vec3 Position, Rotation;
	float Angle;
};