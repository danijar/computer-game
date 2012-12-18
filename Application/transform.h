#pragma once

#include "system.h"

#include <vector>
using namespace std;
#include <GLM/glm.hpp>
using namespace glm;


struct StorageTransform : public Storage
{
	vec3 Position, Rotation;
};