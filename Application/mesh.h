#pragma once

#include <string>
#include "GLEW/glew.h"


struct StorageMesh
{
	StorageMesh() : Changed(true)
	{
		glGenBuffers(1, &Positions);
		glGenBuffers(1, &Normals);
		glGenBuffers(1, &Texcoords);
		glGenBuffers(1, &Elements);
	}

	std::string Path; 
	GLuint Positions, Normals, Texcoords, Elements;
	bool Changed;

	~StorageMesh()
	{
		GLuint buffers[4] = { Positions, Normals, Texcoords, Elements };
		glDeleteBuffers(4, buffers);
	}
};
