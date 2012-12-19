#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageForm : public Storage
{
	unsigned int VerticesPosition, VerticesColor, Elements, Program;
	vec3 Scale;
	StorageForm() : VerticesPosition(-1), VerticesColor(-1), Elements(-1), Program(-1), Scale(1) {}
};
