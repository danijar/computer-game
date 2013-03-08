#pragma once

#include "debug.h"
#include <fstream>
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <GLM/glm.hpp>
using namespace glm;


namespace Shaders
{
	using namespace std;
	using namespace sf;

	GLuint Create(string VertexPath, string FragmentPath);
	bool Test(int Id, bool Output = false);
	bool TestSingle(int Id, bool Output = false);

	void Uniform(GLuint Program, string Name, float Value);
	void Uniform(GLuint Program, string Name, mat4 Value);
	void Uniform(GLuint Program, string Name, vec2 Value);
};