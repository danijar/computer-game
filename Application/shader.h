#pragma once

#include "system.h"

#include <string>
#include <SFML/OpenGL.hpp>

struct StorageShader
{
	StorageShader() : Program(0), ShaderVertex(0), ShaderFragment(0) {}
	
	GLuint Program;
	GLuint ShaderVertex, ShaderFragment;
	std::string PathVertex, PathFragment; // required
	map<std::string, unsigned int> Samplers;
	//map<GLenum, void*> Uniforms; // how to represent that correctly?
};
