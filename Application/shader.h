#pragma once

#include "system.h"

#include <string>
#include <SFML/OpenGL.hpp>

struct StorageShader
{
	StorageShader() : Program(0), ShaderVertex(0), ShaderFragment(0), Changed(true)
	{
		Program = glCreateProgram();
	}
	
	GLuint Program;
	GLuint ShaderVertex, ShaderFragment;
	std::string PathVertex, PathFragment; // required
	map<std::string, unsigned int> Samplers;
	map<GLenum, void*> Uniforms; // how to represent that correctly?
	bool Changed; // uniforms or samplers changed

	~StorageShader()
	{
		if(Program) glDeleteProgram(Program);
	}
};
