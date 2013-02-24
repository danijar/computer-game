#include "shaders.h"


namespace Shaders
{
	GLuint Create(string vertex_path, string fragment_path)
	{
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		string vertex_string((istreambuf_iterator<char>(ifstream(vertex_path))), istreambuf_iterator<char>());
		const GLchar* vertex_chars = vertex_string.c_str();
		glShaderSource(vertex, 1, &vertex_chars, NULL);
		glCompileShader(vertex);

		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		string fragment_string((istreambuf_iterator<char>(ifstream(fragment_path))), istreambuf_iterator<char>());
		const GLchar* fragment_chars = fragment_string.c_str();
		glShaderSource(fragment, 1, &fragment_chars, NULL);
		glCompileShader(fragment);

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		bool result = true;
		if(result) result = TestSingle(vertex  , true); if(!result) Debug::Fail("Shader compilation of (" + vertex_path   + ") fail");
		if(result) result = TestSingle(fragment, true); if(!result) Debug::Fail("Shader compilation of (" + fragment_path + ") fail");
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
}