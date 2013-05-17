#pragma once

#include <GLEW/glew.h>


struct Model
{
	Model() : Positions(0), Normals(0), Texcoords(0), Elements(0) {}

	GLuint Positions, Normals, Texcoords, Elements;
	GLuint Diffuse, Normal, Specular;
};
