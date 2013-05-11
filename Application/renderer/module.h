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

	// shader
	GLuint GetShader(std::string Vertex, std::string Fragment);
	GLuint CreateProgram(std::string Vertex, std::string Fragment);
	GLuint CreateShader(std::string Path, GLenum Type);
	bool TestProgram(int Id);
	bool TestShader(int Id);
	void Uniform(GLuint Program, std::string Name, float Value);
	void Uniform(GLuint Program, std::string Name, glm::mat4 Value);
	void Uniform(GLuint Program, std::string Name, glm::vec2 Value);

	// framebuffer
	std::unordered_map<std::string, GLuint> targets;
	GLuint CreateFramebuffer(std::unordered_map<GLenum, std::pair<GLuint, GLenum>> Targets,
		std::unordered_map<std::string, GLuint> Samplers,
		float Size);
	void TextureResize(GLuint Id, GLenum Type);
	void TextureResize(GLuint Id, GLenum Type, sf::Vector2u Size);
	std::pair<GLenum, GLenum> TextureFormat(GLenum InternalType);

	// passes
	struct Pass
	{
		GLuint Framebuffer;
		GLuint Shader;
		std::string Vertex, Fragment;
		std::unordered_map<std::string, GLuint> Samplers;
		std::unordered_map<GLenum, std::pair<GLuint, GLenum>> Targets;
		float Size;
	};
	std::vector<std::pair<std::string, Pass>> passes;
	void Pipeline();
	void Uniforms();
	Pass CreatePass(
		std::string Name,
		std::string Fragment,
		std::string Target,
		std::pair<std::string, std::string> Sampler,
		float Size = 1.0);
	Pass CreatePass(
		std::string Name,
		std::string Fragment,
		std::string Target,
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		float Size = 1.0);
	Pass CreatePass(
		std::string Name,
		std::string Vertex,
		std::string Fragment,
		std::unordered_map<GLenum, std::pair<std::string, GLenum> > Targets,
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		float Size = 1.0);
	Pass *GetPass(std::string Name);
	
	// draw
	void Quad (Pass *Pass, bool Screen = false);
	void Forms(Pass *Pass);
	void Light(Pass *Pass);

	// effect
	GLuint CreateTexture(std::string Path, bool Repeat = true, bool Filtering = true, bool Mipmapping = true);
};
