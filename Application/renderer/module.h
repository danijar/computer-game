#pragma once

#include "system.h"

#include <string>
#include <unordered_map>
#include <functional>
#include <GLM/glm.hpp>
#include <SFML/System.hpp>


class ModuleRenderer : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

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
	std::unordered_map<std::string, std::tuple<GLuint, GLenum, float>> textures; // name to id, format, size
	GLuint CreateFramebuffer(std::unordered_map<GLenum, GLuint> Targets);
	void TextureCreate(std::string Name, GLenum Type, float Size = 1.0f);
	GLuint TextureGet(std::string Name);
	void TextureResize(GLuint Id, GLenum Type, float Size);
	std::pair<GLenum, GLenum> TextureFormat(GLenum InternalType);

	// passes
	enum Function{ FORMS, LIGHTS, QUAD, SCREEN };
	struct Pass
	{
		Pass() : Enabled(true) {}
		GLuint Framebuffer;
		GLuint Program;
		std::string Vertex, Fragment;
		std::unordered_map<GLenum, GLuint> Targets;
		std::unordered_map<std::string, GLuint> Samplers;
		std::unordered_map<GLuint, GLuint> Fallbacks;
		float Size;
		std::function<void(Pass*)> Function;
		GLenum StencilFunction, StencilOperation; GLint StencilReference;
		bool Enabled;
	};
	std::vector<std::pair<std::string, Pass>> passes;
	void Pipeline();
	void Uniforms();
	void CreatePass(std::string Name,
		std::string Vertex, std::string Fragment,
		std::unordered_map<GLenum,      std::string> Targets,
		std::unordered_map<std::string, std::string> Samplers,
		std::unordered_map<std::string, std::string> Fallbacks,
		Function Function = QUAD, float Size = 1.0,
		GLenum StencilFunction = GL_ALWAYS, GLint StencilReference = 0, GLenum StencilOperation = GL_KEEP);
	Pass *GetPass(std::string Name);
	
	// draw
	void DrawQuad(Pass *Pass);
	void DrawQuadStenciled(Pass *Pass);
	void DrawScreen(Pass *Pass);
	void DrawForms(Pass *Pass);
	void DrawSky(Pass *Pass);
	void DrawLights(Pass *Pass);

	// effect
	GLuint CreateTexture(std::string Path, bool Repeat = true, bool Filtering = true, bool Mipmapping = true);

public:
	// callbacks
	static v8::Handle<v8::Value> jsPass     (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsWireframe(const v8::Arguments& args);
};
