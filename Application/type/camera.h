#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Camera
{
	Camera() : Active(false), Person(0), Pitch(0.f)
	{
		View = glm::lookAt(
			glm::vec3(1.2, 1.2, 1.2),
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(0.0, 0.0, 1.0)
		);
	}

	bool Active;
	uint64_t Person;
	float Pitch;
	glm::vec2 Angles;
	glm::mat4 View, Projection;
};
