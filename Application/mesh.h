#pragma once

#include <string>
#include "GLEW/glew.h"


struct StorageMesh
{
	StorageMesh() : Changed(true)
	{
		glGenBuffers(4, &Positions);
		glGenBuffers(4, &Normals);
		glGenBuffers(4, &Texcoords);
		glGenBuffers(4, &Elements);
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
