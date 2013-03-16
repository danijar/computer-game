#pragma once

#include "system.h"

#include <GLEW/glew.h>
#include <GLM/glm.hpp>
using namespace glm;


struct StorageForm
{
	StorageForm() : Texture(0) {}
	GLuint Vertices, Normals, Texcoords, Elements, Texture;
};
