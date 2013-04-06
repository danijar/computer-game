#pragma once

#include "module.h"

using namespace std;
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "shader.h"


GLuint ModuleRenderer::CreateProgram(string PathVertex, string PathFragment)
{
	GLuint vertex   = CreateShader(PathVertex,   GL_VERTEX_SHADER  );
	GLuint fragment = CreateShader(PathFragment, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex  );
	glAttachShader(program, fragment);
	glLinkProgram(program);

	if(!TestProgram(program))
	{
		Debug->Fail("program creation fail");
		return 0;
	}

	glDetachShader(program, vertex  );
	glDetachShader(program, fragment);
	glDeleteShader(vertex  );
	glDeleteShader(fragment);
	return program;
}

GLuint ModuleRenderer::CreateShader(string Path, GLenum Type)
{
	GLuint id = glCreateShader(Type);
	string source = HelperFile::Read("renderer/shader", Path);
	const GLchar* chars = source.c_str();
	glShaderSource(id, 1, &chars, NULL);
	glCompileShader(id);
	if(!TestShader(id)) Debug->Fail("... in (" + Path + ")");
	return id;
}

bool ModuleRenderer::TestProgram(int Id)
{
	GLint Status;
	glGetProgramiv(Id, GL_LINK_STATUS, &Status);
	bool Result = (Status == GL_TRUE);
	if(!Result)
	{
		GLchar Log[513];
		GLsizei Length;
		glGetProgramInfoLog(Id, 512, &Length, Log);
		if(Length > 0) Debug->Fail(Log);
	}
	return Result;
}

bool ModuleRenderer::TestShader(int Id)
{
	GLint Status;
	glGetShaderiv(Id, GL_COMPILE_STATUS, &Status);
	bool Result = (Status == GL_TRUE);
	if(!Result)
	{
		GLchar Log[513];
		GLsizei Length;
		glGetShaderInfoLog(Id, 512, &Length, Log);
		if(Length > 0) Debug->Fail(Log);
	}
	return Result;
}

void ModuleRenderer::Uniform(GLuint Program, string Name, float Value)
{
	glUseProgram(Program);
	glUniform1f(glGetUniformLocation(Program, Name.c_str()), Value);
	glUseProgram(0);
}

void ModuleRenderer::Uniform(GLuint Program, string Name, mat4 Value)
{
	glUseProgram(Program);
	glUniformMatrix4fv(glGetUniformLocation(Program, Name.c_str()), 1, GL_FALSE, value_ptr(Value));
	glUseProgram(0);
}

void ModuleRenderer::Uniform(GLuint Program, string Name, vec2 Value)
{
	glUseProgram(Program);
	glUniform2fv(glGetUniformLocation(Program, Name.c_str()), 1, value_ptr(Value));
	glUseProgram(0);
}
