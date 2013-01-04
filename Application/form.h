#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageForm : public Storage
{
	unsigned int Positions, Normals, Texcoords, Elements, Program, Texture;
	vec3 Scale;
	StorageForm() : Scale(1) {}
};
