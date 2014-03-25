#pragma once

#include <string>
#include <glew/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct Model
{
	Model() : Positions(0), Normals(0), Texcoords(0), Elements(0), Shininess(0.0f), Diffuse(0), Normal(0), Specular(0), Box() {}

	// vertices
	GLuint Positions, Normals, Texcoords, Elements;

	// material
	std::string Material;
	GLfloat Shininess;
	GLuint Diffuse, Normal, Specular;

	// bounding box
	struct Bounds
	{
		struct Range { Range() : Min(0), Max(0) {} float Min, Max; };
		Bounds() : X(), Y(), Z() {}
		Range X, Y, Z;
	};
	Bounds Box;
};
