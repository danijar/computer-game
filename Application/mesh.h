#pragma once

#include <string>
#include "GLEW/glew.h"


struct StorageMesh
{
	StorageMesh() : Changed(true) {}

	std::string Path; 
	GLuint Positions, Normals, Texcoords, Elements;
	bool Changed;
};
