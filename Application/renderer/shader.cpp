#include "module.h"

using namespace std;


GLuint ModuleRenderer::CreateProgram(string Vertex, string Fragment)
{
	GLuint vertex   = CreateShader(Vertex, GL_VERTEX_SHADER);
	GLuint fragment = CreateShader(Fragment, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	if(!TestProgram(program))
	{
		Debug->Fail("program creation fail");
		return 0;
	}

	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return program;
}

GLuint ModuleRenderer::CreateShader(string Path, GLenum Type)
{
	GLuint id = glCreateShader(Type);
	string source = File->Read("shader/" + Path);
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
