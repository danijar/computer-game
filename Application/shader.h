#pragma once

#include "system.h"

#include <SFML/OpenGL.hpp>
using namespace sf;

struct StorageShader : public Storage
{
	GLuint Program;
};