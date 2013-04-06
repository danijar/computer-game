#pragma once

#include "system.h"

#include <string>
#include <unordered_map>
#include <SFML/OpenGL.hpp>


class ModuleRenderer : public Module
{
	void Init();
	void Update();
	void Listeners();

	void Quad (GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);
	void Forms(GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);
	void Light(GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);
};
