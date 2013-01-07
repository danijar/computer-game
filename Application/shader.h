#pragma once

#include "system.h"

#include <SFML/OpenGL.hpp>
using namespace sf;

struct StorageShader : public Storage
{
	GLuint Vertex, Fragment, Program,									// shaders
		   AtrVertex, AtrNormal, AtrTexcoord,							// inputs
	       UniVertex, UniNormal, UniView, UniProjection, UniTexture;	// uniforms
};
