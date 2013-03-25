#pragma once

#include <GLEW/glew.h>
#include <GLM/glm.hpp>
using namespace glm;


struct StorageForm
{
	GLuint Vertices, Normals, Texcoords, Elements;
	unsigned int Material;

	// instead just store...
	// unsigned int Mesh, Material;
};
