#pragma once

#include "system.h"

#include <GLEW/glew.h>


struct StorageTexture
{
	StorageTexture() : Id(0), Mipmapping(true), Changed(true)
	{
		glGenTextures(1, &Id);
	}

	GLuint Id;
	std::string Path; // required
	bool Mipmapping;
	bool Changed;

	~StorageTexture()
	{
		if(Id) glDeleteTextures(1, &Id);
	}
};
