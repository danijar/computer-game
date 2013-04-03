#pragma once

#include <GLEW/glew.h>


struct StorageTexture
{
	StorageTexture() : Mipmapping(true), Changed(true)
	{
		glGenTextures(1, &Id);
	}

	GLuint Id;
	std::string Path;
	bool Mipmapping;
	bool Changed;

	~StorageTexture()
	{
		glDeleteTextures(1, &Id);
	}
};
