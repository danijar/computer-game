#pragma once

#include "system.h"

#include <SFML/OpenGL.hpp>


struct StorageBuffers : public Storage
{
	GLuint VertexArray, VertexBuffer, ElementBuffer, TextureBuffer;
};
