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
	std::unordered_map<std::string, GLuint> targets;
	GLuint CreateFramebuffer(std::unordered_map<GLenum, std::pair<GLuint, GLenum>> Targets, float Size);
	void TextureResize(GLuint Id, GLenum Type);
	void TextureResize(GLuint Id, GLenum Type, sf::Vector2u Size);
	std::pair<GLenum, GLenum> TextureFormat(GLenum InternalType);

	// passes
	typedef std::unordered_map<GLenum, std::pair<std::string, GLenum> > TargetList;
	typedef std::unordered_map<std::string, std::string> SamplerList;
	struct Pass
	{
		GLuint Framebuffer;
		GLuint Shader;
		std::string Vertex, Fragment;
		std::unordered_map<GLenum, std::pair<GLuint, GLenum>> Targets;
		std::unordered_map<std::string, GLuint> Samplers;
		float Size;
		std::function<void(Pass*)> Drawfunction;
		GLenum StencilFunction;
		GLint StencilReference;
	};
	std::vector<std::pair<std::string, Pass>> passes;
	void Pipeline();
	void Uniforms();
	Pass CreatePass(std::string Name,
		std::string Fragment,
		TargetList Targets, SamplerList Samplers,
		std::function<void(Pass*)> Drawfunction,
		float Size = 1.0,
		GLenum StencilFunction = GL_ALWAYS, GLint StencilReference = 0);
	Pass CreatePass(std::string Name,
		std::string Vertex, std::string Fragment,
		TargetList Targets, SamplerList Samplers,
		std::function<void(Pass*)> Drawfunction,
		float Size = 1.0,
		GLenum StencilFunction = GL_ALWAYS, GLint StencilReference = 0);
	Pass *GetPass(std::string Name);
	TargetList Targets(GLenum attachment1 = 0, std::string texture1 = "", GLenum type1 = GL_RGB16,
		GLenum attachment2 = 0, std::string texture2 = "", GLenum type2 = GL_RGB16,
		GLenum attachment3 = 0, std::string texture3 = "", GLenum type3 = GL_RGB16,
		GLenum attachment4 = 0, std::string texture4 = "", GLenum type4 = GL_RGB16);
	SamplerList Samplers(std::string sampler1 = "", std::string texture1 = "",
		std::string sampler2 = "", std::string texture2 = "",
		std::string sampler3 = "", std::string texture3 = "",
		std::string sampler4 = "", std::string texture4 = "");
	
	// draw
	void DrawQuad(Pass *Pass);
	void DrawQuadStenciled(Pass *Pass);
	void DrawQuadScreen(Pass *Pass);
	void DrawForms(Pass *Pass);
	void DrawSky(Pass *Pass);
	void DrawLight(Pass *Pass);

	// effect
	GLuint CreateTexture(std::string Path, bool Repeat = true, bool Filtering = true, bool Mipmapping = true);

public:
	// callbacks
	static v8::Handle<v8::Value> jsPass     (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsWireframe(const v8::Arguments& args);
};
