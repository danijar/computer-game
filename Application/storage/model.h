#pragma once

#include <string>
#include <GLEW/glew.h>


struct Model
{
	Model() : Positions(0), Normals(0), Texcoords(0), Elements(0) {}

	GLuint Positions, Normals, Texcoords, Elements;
	std::string Material;
	GLuint Diffuse, Normal, Specular;
};
