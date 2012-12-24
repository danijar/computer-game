#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageTransform : public Storage
{
	vec3 Position, Rotation;
};