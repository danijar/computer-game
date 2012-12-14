#pragma once

#include "system.h"

#include <SFML/OpenGL.hpp>
using namespace sf;

struct StorageShader : public Storage
{
	GLuint Vertex, Fragment, Program;
	GLint Position, Color, Model, View, Projection;
	StorageShader() : Vertex(-1), Fragment(-1), Program(-1), Position(-1), Color(-1), Model(-1) {}
};