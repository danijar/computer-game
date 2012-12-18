#pragma once

#include "system.h"

#include <GLM/glm.hpp>
using namespace glm;


struct StorageForm : public Storage
{
	unsigned int VertexBuffer, ElementBuffer;
	vec3 Scale;
	StorageForm() : VertexBuffer(-1), ElementBuffer(-1), Scale(1) {}
};
