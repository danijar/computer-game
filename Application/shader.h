#pragma once

#include <string>
#include <unordered_map>
#include <SFML/OpenGL.hpp>

struct StorageShader
{
	StorageShader() : Program(0), ShaderVertex(0), ShaderFragment(0) {}
	
	GLuint Program;
	GLuint ShaderVertex, ShaderFragment;
	std::string PathVertex, PathFragment; // required
	unordered_map<std::string, GLuint> Samplers;
	//map<GLenum, void*> Uniforms; // how to represent that correctly?

	void Paths(std::string Vertex, std::string Fragment)
	{
		PathVertex = Vertex;
		PathFragment = Fragment;
	}
};
