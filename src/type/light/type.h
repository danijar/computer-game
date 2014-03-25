#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct Light
{
	Light() : Type(POINT), Color(glm::vec3(1)), Intensity(1.0f) {}
	enum Shape{ POINT, SPOT, DIRECTIONAL } Type;

	glm::vec3 Color;
	float Radius, Intensity;
};
