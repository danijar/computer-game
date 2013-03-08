#include "shaders.h"

#include <GLM/gtc/type_ptr.hpp>


namespace Shaders
{
	GLuint Create(string VertexPath, string FragmentPath)
	{
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		string vertex_string((istreambuf_iterator<char>(ifstream(VertexPath))), istreambuf_iterator<char>());
		const GLchar* vertex_chars = vertex_string.c_str();
		glShaderSource(vertex, 1, &vertex_chars, NULL);
		glCompileShader(vertex);

		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		string fragment_string((istreambuf_iterator<char>(ifstream(FragmentPath))), istreambuf_iterator<char>());
		const GLchar* fragment_chars = fragment_string.c_str();
		glShaderSource(fragment, 1, &fragment_chars, NULL);
		glCompileShader(fragment);

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		bool result = true;
		if(result) result = TestSingle(vertex  , true); if(!result) Debug::Fail("Shader compilation of (" + VertexPath   + ") fail");
		if(result) result = TestSingle(fragment, true); if(!result) Debug::Fail("Shader compilation of (" + FragmentPath + ") fail");
		if(result) result = Test(program);
		Debug::PassFail("Shader creation", result);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return program;
	}

	bool Test(int Id, bool Output)
	{
		GLint Success;
		glGetProgramiv(Id, GL_LINK_STATUS, &Success);
		bool Result = (Success == GL_TRUE);

		GLchar Log[513];
		GLsizei Length;
		glGetProgramInfoLog(Id, 512, &Length, Log);
		if(Length > 0 && (Output || !Result)) Debug::PassFail(Log, Result, "", "");

		return Result;
	}

	bool TestSingle(int Id, bool Output)
	{
		GLint Success;
		glGetShaderiv(Id, GL_COMPILE_STATUS, &Success);
		bool Result = (Success == GL_TRUE);

		GLchar Log[513];
		GLsizei Length;
		glGetShaderInfoLog(Id, 512, &Length, Log);
		if(Length > 0 && (Output || !Result)) Debug::PassFail(Log, Result, "", "");

		return Result;
	}

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
}