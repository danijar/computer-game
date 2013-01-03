#pragma once

#include "system.h"

#include <SFML/OpenGL.hpp>
using namespace sf;

struct StorageShader : public Storage
{
	GLuint Vertex, Fragment, Program,			// shaders
		   Position, Texcoord,					// inputs
	       Model, View, Projection, Texture;	// uniforms
};