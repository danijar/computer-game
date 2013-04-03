#pragma once

#include "GLM/glm.hpp"


struct StorageLight
{
	StorageLight() : Intensity(1.f), Type(POINT) {}
	glm::vec3 Color;
	float Radius, Intensity;
	enum Shape{ POINT, SPOT, DIRECTIONAL } Type;
};
