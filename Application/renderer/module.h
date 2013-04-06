#pragma once

#include "system.h"

#include <string>
#include <unordered_map>
#include <GLM/glm.hpp>
#include <SFML/System.hpp>


class ModuleRenderer : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// draw
	void Quad (GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);
	void Forms(GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);
	void Light(GLuint Shader, std::unordered_map<std::string, GLuint> Samplers);

	// shader
	GLuint CreateProgram(std::string PathVertex, std::string PathFragment);
	GLuint CreateShader(std::string Path, GLenum Type);
	bool TestProgram(int Id);
	bool TestShader(int Id);
	void Uniform(GLuint Program, std::string Name, float Value);
	void Uniform(GLuint Program, std::string Name, glm::mat4 Value);
	void Uniform(GLuint Program, std::string Name, glm::vec2 Value);

	// framebuffer
	void Resize();
	void Resize(sf::Vector2u Size);
	void Setup(unsigned int Id);
	void TextureResize(GLuint Id, GLenum Type);
	void TextureResize(GLuint Id, GLenum Type, sf::Vector2u Size);
	std::pair<GLenum, GLenum> TextureFormat(GLenum InternalType);

	// pipeline
	std::unordered_map<std::string, GLuint> Textures;
	GLuint forms, occlusion, combine, antialiasing;
	void Pipeline();
	void Uniforms();
	unsigned int CreatePass(
		std::string Fragment,
		std::string Target,
		std::pair<std::string, std::string> Sampler,
		float Size = 1.0);
	unsigned int CreatePass(
		std::string Fragment,
		std::string Target,
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		float Size = 1.0);
	unsigned int CreatePass(
		std::string Vertex,
		std::string Fragment,
		std::unordered_map<GLenum, std::pair<std::string, GLenum> > Targets,
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		float Size = 1.0);
};
