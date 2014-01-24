#pragma once

#include <string>
#include <glew/glew.h>


struct Model
{
	Model() : Positions(0), Normals(0), Texcoords(0), Elements(0), Shininess(0.0f), Diffuse(0), Normal(0), Specular(0) {}

	GLuint Positions, Normals, Texcoords, Elements;
	std::string Material;
	GLfloat Shininess;
	GLuint Diffuse, Normal, Specular;
};
