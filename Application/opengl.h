#pragma once

#include "debug.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <string>


namespace Opengl
{
	using namespace std;

	bool InitGlew();
	int Test();
};