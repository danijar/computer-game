#pragma once

#include "system/system.h"

#include <string>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>
#include <sfml/System.hpp>


class ModuleRenderer : public Module
{
	// general
	GLuint query;
	GLuint64 time;
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

	// framebuffer
	std::unordered_map<std::string, std::tuple<GLuint, GLenum, float>> textures; // name to id, format, size
	GLuint FramebufferCreate(std::unordered_map<GLenum, GLuint> Targets);
	void FramebufferTarget(GLuint Id, GLenum Attachment, GLuint Target, bool Output = true);
	void FramebufferTargets(GLuint Id, std::unordered_map<GLenum, GLuint> Targets, bool Output = true);
	void TextureCreate(std::string Name, GLenum Type = GL_RGB16F, float Size = 1.0f);
	std::tuple<GLuint, GLenum, float> TextureGet(std::string Name);
	void TextureResize(GLuint Id, GLenum Type, float Size);
	std::pair<GLenum, GLenum> TextureFormat(GLenum InternalType);

	// passes
	enum Function{ FORMS, SKY, LIGHTS, QUAD, SCREEN };
	struct Pass
	{
		Pass() : Enabled(true), Time(0) {}
		GLuint Framebuffer;
		GLuint Program;
		std::string Vertex, Fragment;
		std::unordered_map<GLenum, GLuint> Targets;
		std::unordered_map<std::string, GLuint> Samplers;
		std::unordered_map<GLuint, GLuint> Copyfallbacks;
		std::unordered_map<GLuint, glm::vec3> Colorfallbacks;
		std::function<void(Pass*)> Function;
		bool Clear, Depth;
		glm::vec3 Clearcolor;
		float Size;
		GLenum StencilFunction, StencilOperation; GLint StencilReference;
		bool Enabled;
		// profiling
		GLuint Query;
		GLuint64 Time;
	};
	std::vector<std::pair<std::string, Pass>> passes;
	Pass color, copy;
	void PassCreate(std::string Name,
		std::string Vertex, std::string Fragment,
		std::unordered_map<GLenum, std::string> Targets = std::unordered_map<GLenum, std::string>(),
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		std::unordered_map<std::string, std::string> Copyfallbacks = std::unordered_map<std::string, std::string>(),
		std::unordered_map<std::string, glm::vec3> Colorfallbacks = std::unordered_map<std::string, glm::vec3>(),
		Function Function = QUAD, bool Depth = false, bool Clear = false, glm::vec3 Clearcolor = glm::vec3(0), float Size = 1.0,
		GLenum StencilFunction = GL_ALWAYS, GLint StencilReference = 0, GLenum StencilOperation = GL_KEEP);
	Pass PassCreate(
		std::string Vertex, std::string Fragment,
		std::unordered_map<GLenum, std::string> Targets = std::unordered_map<GLenum, std::string>(),
		std::unordered_map<std::string, std::string> Samplers = std::unordered_map<std::string, std::string>(),
		std::unordered_map<std::string, std::string> Copyfallbacks = std::unordered_map<std::string, std::string>(),
		std::unordered_map<std::string, glm::vec3> Colorfallbacks = std::unordered_map<std::string, glm::vec3>(),
		Function Function = QUAD, bool Depth = false, bool Clear = false, glm::vec3 Clearcolor = glm::vec3(0), float Size = 1.0,
		GLenum StencilFunction = GL_ALWAYS, GLint StencilReference = 0, GLenum StencilOperation = GL_KEEP);
	Pass *PassGet(std::string Name, bool Output = true);
	
	// draw
	void DrawForms(Pass *Pass);
	void DrawSky(Pass *Pass);
	void DrawLights(Pass *Pass);
	void DrawQuad(Pass *Pass);
	void DrawScreen(Pass *Pass);

	// culling
	bool Visible(uint64_t Id);

	// effect
	void TextureLoad(std::string Name, std::string Path, bool Repeat = true, bool Filtering = true, bool Mipmapping = true);

public:
	// callbacks
	static v8::Handle<v8::Value> jsRenderpass      (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRendertarget    (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRendertargetload(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsUniform         (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsWireframe       (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsProjection      (const v8::Arguments& args);
};
