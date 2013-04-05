#pragma once

#include <string>
#include <SFML/OpenGL.hpp>


struct StorageModel
{
	GLuint Positions, Normals, Texcoords, Elements;
	GLuint Diffuse, Normal, Specular;
};
