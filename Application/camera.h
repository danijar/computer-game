#pragma once

#include "system.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;


struct StorageCamera
{
	StorageCamera() : Active(false)
	{
		View = lookAt(
			vec3(1.2, 1.2, 1.2),
			vec3(0.0, 0.0, 0.0),
			vec3(0.0, 0.0, 1.0)
		);
	}
	bool Active;
	vec3 Position;
	vec2 Angles;
	mat4 View;
};