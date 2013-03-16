#pragma once

#include "system.h"
#include "debug.h"

#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;
#include <fstream>
using namespace std;

#include "shader.h"


class ComponentShader : public Component
{
	void Init()
	{
		/*
		 * where to get the needed shaders?
		 *
		 * How guarantee that shaders are valid soon enough to not crash the renderer?
		 * Ideas: - let components create shaders by sending events
		 *        - use store a changed flag in the storage and update all with the flag set every frame
		          - when doing so, how to distinct whether the source or simply an uniform changed?
		 */

		Listeners();
	}

	void Update()
	{

	}

	void Listeners()
	{
		// on window get focus, look for modified source files
	}

	////////////////////////////////////////////////////////////
	// Creation
	////////////////////////////////////////////////////////////

	GLuint Create(string VertexPath, string FragmentPath)
	{
		GLuint vertex   = CreateShader(VertexPath,   GL_VERTEX_SHADER  );
		GLuint fragment = CreateShader(FragmentPath, GL_FRAGMENT_SHADER);

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex  );
		glAttachShader(program, fragment);
		glLinkProgram(program);

		Debug::PassFail("Shader creation", TestProgram(program));

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return program;
	}

	GLuint CreateShader(string Path, GLenum Type)
	{
		GLuint id = glCreateShader(Type);
		string source = string((istreambuf_iterator<char>(ifstream(Path))), istreambuf_iterator<char>());
		const GLchar* chars = source.c_str();
		glShaderSource(id, 1, &chars, NULL);
		glCompileShader(id);
		if(!TestShader(id)) Debug::Info("... in (" + Path + ")");
		return id;
	}

	////////////////////////////////////////////////////////////
	// Testing
	////////////////////////////////////////////////////////////

	bool TestProgram(int Id)
	{
		GLint Status;
		glGetProgramiv(Id, GL_LINK_STATUS, &Status);
		bool Result = (Status == GL_TRUE);
		if(!Result)
		{
			GLchar Log[513];
			GLsizei Length;
			glGetProgramInfoLog(Id, 512, &Length, Log);
			if(Length > 0) Debug::Fail(Log);
		}
		return Result;
	}

	bool TestShader(int Id)
	{
		GLint Status;
		glGetShaderiv(Id, GL_COMPILE_STATUS, &Status);
		bool Result = (Status == GL_TRUE);
		if(!Result)
		{
			GLchar Log[513];
			GLsizei Length;
			glGetShaderInfoLog(Id, 512, &Length, Log);
			if(Length > 0) Debug::Fail(Log);
		}
		return Result;
	}

	////////////////////////////////////////////////////////////
	// Uniforms
	////////////////////////////////////////////////////////////

	void Uniform(GLuint Program, string Name, float Value)
	{
		glUseProgram(Program);
		glUniform1f(glGetUniformLocation(Program, Name.c_str()), Value);
		glUseProgram(0);
	}

	void Uniform(GLuint Program, string Name, mat4 Value)
	{
		glUseProgram(Program);
		glUniformMatrix4fv(glGetUniformLocation(Program, Name.c_str()), 1, GL_FALSE, value_ptr(Value));
		glUseProgram(0);
	}

	void Uniform(GLuint Program, string Name, vec2 Value)
	{
		glUseProgram(Program);
		glUniform2fv(glGetUniformLocation(Program, Name.c_str()), 1, value_ptr(Value));
		glUseProgram(0);
	}
};
