#pragma once

#include "system.h"

#include <string>
#include <GLEW/glew.h>


class ModuleSky : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// create
	struct Mesh { GLuint Positions, Normals, Texcoords, Elements; };
	unsigned int CreateSky();
	Mesh CreateMesh(int Steps = 10, float Radius = 999.0f);
	GLuint CreateTexture(std::string Path);
};
