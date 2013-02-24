#pragma once

#include "debug.h"
#include <fstream>
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>


namespace Shaders
{
	using namespace std;
	using namespace sf;

	GLuint Create(string vertex_path, string fragment_path);
	bool Test(int Id, bool Output = false);
	bool TestSingle(int Id, bool Output = false);
};