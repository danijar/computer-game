#pragma once

#include "system.h"

#include <GLEW/glew.h>


struct StorageFramebuffer
{
	GLuint Id;
	std::map<GLenum, unsigned int> Targets; // map attachment point to system id of texture
	bool Changed;
};
