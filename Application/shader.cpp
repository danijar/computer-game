#pragma once

#include "system.h"
#include "debug.h"

#include <fstream>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
using namespace sf;

#include "shader.h"


class ComponentShader : public Component
{
	void Init()
	{
		Global->Add<StorageShader>("shader");

		Create("shaders/basic.vert", "shaders/basic.frag");
	}

	void Update()
	{

	}

	void Create(string PathVertex, string PathFragment)
	{
		auto shd = Global->Get<StorageShader>("shader");

		shd->Vertex = glCreateShader(GL_VERTEX_SHADER);
		string VertexString((istreambuf_iterator<char>(ifstream(PathVertex))), istreambuf_iterator<char>());
		const GLchar* VertexChars = VertexString.c_str();
		glShaderSource(shd->Vertex, 1, &VertexChars, NULL);
		glCompileShader(shd->Vertex);

		shd->Fragment = glCreateShader(GL_FRAGMENT_SHADER);
		string FragmentString((istreambuf_iterator<char>(ifstream(PathFragment))), istreambuf_iterator<char>());
		const GLchar* FragmentChars = FragmentString.c_str();
		glShaderSource(shd->Fragment, 1, &FragmentChars, NULL);
		glCompileShader(shd->Fragment);

		shd->Program = glCreateProgram();
		glAttachShader(shd->Program, shd->Vertex);
		glAttachShader(shd->Program, shd->Fragment);
		glLinkProgram(shd->Program);

		Debug::PassFail("Shader creation", ShaderTest(shd->Vertex) && ShaderTest(shd->Fragment) && ProgramTest(shd->Program));

		glDeleteShader(shd->Vertex);
		glDeleteShader(shd->Fragment);

		Attributes();
	}

	void Attributes()
	{
		auto shd = Global->Get<StorageShader>("shader");

		shd->UniView       = glGetUniformLocation(shd->Program, "view_mat"  );
		shd->UniProjection = glGetUniformLocation(shd->Program, "proj_mat"  );
		shd->UniVertex     = glGetUniformLocation(shd->Program, "vertex_mat");
		shd->UniNormal     = glGetUniformLocation(shd->Program, "normal_mat");
		shd->UniTexture    = glGetUniformLocation(shd->Program, "tex"       );
		shd->AtrVertex     = glGetAttribLocation (shd->Program, "vertex"    );
		shd->AtrNormal     = glGetAttribLocation (shd->Program, "normal"    );
		shd->AtrTexcoord   = glGetAttribLocation (shd->Program, "texcoord"  );
	}

	bool ProgramTest(int Id, bool Output = false)
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

	bool ShaderTest(int Id, bool Output = false)
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

};