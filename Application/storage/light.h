#pragma once

#include <GLM/glm.hpp>


struct StorageLight
{
	StorageLight() : Intensity(1.f), Type(POINT) {}
	enum Shape{ POINT, SPOT, DIRECTIONAL } Type;

	glm::vec3 Color;
	float Radius, Intensity;
};
