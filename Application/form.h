#pragma once

#include "system.h"

#include <vector>
using namespace std;
#include <SFML/OpenGL.hpp>
using namespace sf;
#include <GLM/glm.hpp>
using namespace glm;


struct StorageForms : public Storage
{
	struct Form
	{
		GLuint VertexBuffer, ElementBuffer;
		vec3 Position, Rotation, Scale;
		Form() : VertexBuffer(-1), ElementBuffer(-1), Scale(1) {}
	};

	vector<Form> List;
};

