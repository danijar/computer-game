#pragma once

#include "system.h"

#include <vector>
using namespace std;
#include <SFML/OpenGL.hpp>
using namespace sf;
#include <GLM/glm.hpp>
using namespace glm;


struct Form
{
	GLuint VertexBuffer, ElementBuffer;
	vec3 Position, Rotation, Scale;
	Form() : VertexBuffer(-1), ElementBuffer(-1), Scale(1) {}
};

struct StorageForms : public Storage
{
	vector<Form> List;
};

struct Movable
{
	// vec3 Momentum;
};

struct StorageMovables : public Storage
{
	vector<Movable> List;
};