#pragma once

#include "GLM/glm.hpp"


struct StorageLight
{
	StorageLight() : Intensity(1.0f), Type(POINT) {}
	glm::vec3 Position;
	glm::vec3 Color;
	float Radius, Intensity;
	enum Type{ POINT, SPOT, DIRECTIONAL } Type;
};
