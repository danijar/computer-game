#pragma once

#include "GLM/glm.hpp"


struct StorageLight
{
	enum Type{ POINT, SPOT, DIRECTIONAL } Type;
	glm::vec3 Color;
};
