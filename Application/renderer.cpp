#pragma once

#include "system.h"
#include "debug.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;
#include <fstream>
using namespace std;

#include "settings.h"
#include "window.h"
#include "shader.h"
#include "form.h"


class ComponentRenderer : public Component
{

	void Init()
	{
		glewExperimental = GL_TRUE;
		glewInit();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Shader("shaders/vertex.txt", "shaders/fragment.txt");

		Window();
		Perspective();

		Listeners();
	}

	void Update()
	{
		auto shd = Storage->Get<StorageShader>("shader");
		auto fms = Storage->Get<StorageForms>("forms");
		auto cube = Storage->Get<StorageForm>("cube");
		float time = clock.getElapsedTime().asSeconds();

		glClearColor(.4f,.6f,.9f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(shd->Position);
		glVertexAttribPointer(shd->Position, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
		glEnableVertexAttribArray(shd->Color);
		glVertexAttribPointer(shd->Color, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

		mat4 View = lookAt(
			vec3(1.2, 1.2, 1.2),
			vec3(0.0, 0.0, 0.0),
			vec3(0.0, 0.0, 1.0)
		);
		glUniformMatrix4fv(shd->View, 1, GL_FALSE, value_ptr(View));

		Draw(cube);
		for(auto i = fms->List.begin(); i != fms->List.end(); ++i)
		{
			Draw(&fms->List[0] + (i - fms->List.begin()));
		}
	}

	Clock clock;

	void Listeners()
	{
		Event->Listen("WindowRecreated", [=]{
			Window();
			Perspective();
			Attributes();
		});

		Event->Listen<Vector2i>("WindowResize", [=](Vector2i Size){
			Perspective(Size);
		});
	}

	void Draw(StorageForm* i)
	{
		auto shd = Storage->Get<StorageShader>("shader");
		float time = clock.getElapsedTime().asSeconds();

		mat4 Scale		= scale(mat4(1), i->Scale);
		mat4 Translate	= translate(mat4(1), i->Position);
		mat4 Rotate		= rotate(mat4(1), time * 360, i->Rotation);
		mat4 Model = Translate * Rotate * Scale;

		glUniformMatrix4fv(shd->Model, 1, GL_FALSE, value_ptr(Model));	// maybe draw later

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	void Window()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		auto pgr = Storage->Get<StorageShader>("shader")->Program;
		
		wnd->setVerticalSyncEnabled(true);

		glEnable(GL_DEPTH_TEST);
		glUseProgram(pgr);
	}

	void Perspective()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;

		Perspective((Vector2i)wnd->getSize());
	}

	void Perspective(Vector2i Size)
	{
		auto shd = Storage->Get<StorageShader>("shader");

		glViewport(0, 0, Size.x, Size.y);

		mat4 Projection = perspective(45.0f, (float)Size.x / (float)Size.y, 1.0f, 100.0f);
		glUniformMatrix4fv(shd->Projection, 1, GL_FALSE, value_ptr(Projection));
	}

	void Shader(string PathVertex, string PathFragment)
	{
		auto shd = Storage->Add<StorageShader>("shader");

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
		auto shd = Storage->Get<StorageShader>("shader");

		shd->View = glGetUniformLocation(shd->Program, "view");
		shd->Projection = glGetUniformLocation(shd->Program, "proj");
		shd->Model = glGetUniformLocation(shd->Program, "model");
		shd->Position = glGetAttribLocation(shd->Program, "position");
		shd->Color = glGetAttribLocation(shd->Program, "color");
	}

	bool ProgramTest(int Id, bool Output = false)
	{
		GLint Success;
		glGetProgramiv(Id, GL_LINK_STATUS, &Success);
		bool Result = (Success == GL_TRUE);
		if(Output)
		{
			GLchar Log[513];
			GLsizei Length;
			glGetProgramInfoLog(Id, 512, &Length, Log);
			if(Length > 0) Debug::PassFail(Log, Result, "", "");
		}
		return Result;
	}

	bool ShaderTest(int Id, bool Output = false)
	{
		GLint Success;
		glGetShaderiv(Id, GL_COMPILE_STATUS, &Success);
		bool Result = (Success == GL_TRUE);
		if(Output)
		{
			GLchar Log[513];
			GLsizei Length;
			glGetShaderInfoLog(Id, 512, &Length, Log);
			if(Length > 0) Debug::PassFail(Log, Result, "", "");
		}
		return Result;
	}

};