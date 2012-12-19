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
#include "transform.h"


class ComponentRenderer : public Component
{
	void Init()
	{
		glewExperimental = GL_TRUE;
		glewInit();

		Shader("shaders/vertex.txt", "shaders/fragment.txt");
		Window();

		Listeners();
	}

	void Update()
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto fms = Entity->Get<StorageForm>();

		glClearColor(.4f,.6f,.9f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 View = lookAt(
			vec3(1.2, 1.2, 1.2),
			vec3(0.0, 0.0, 0.0),
			vec3(0.0, 0.0, 1.0)
		);
		glUniformMatrix4fv(shd->View, 1, GL_FALSE, value_ptr(View));

		for(auto i = fms.begin(); i != fms.end(); ++i) Draw(i->first);
	}

	void Listeners()
	{
		Event->Listen("WindowRecreated", [=]{
			Window();
		});

		Event->Listen<Vector2i>("WindowResize", [=](Vector2i Size){
			Perspective(Size);
		});
	}

	void Draw(unsigned int id)
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto frm = Entity->Get<StorageForm>(id);
		auto tsf = Entity->Get<StorageTransform>(id);

		glUseProgram(frm->Program);

		mat4 Scale		= scale(mat4(1), frm->Scale);
		mat4 Translate	= translate(mat4(1), tsf->Position);
		mat4 Rotate		= rotate(mat4(1), tsf->Angle, tsf->Rotation);
		mat4 Model = Translate * Rotate * Scale;
		glUniformMatrix4fv(shd->Model, 1, GL_FALSE, value_ptr(Model));

		glEnableVertexAttribArray(shd->Position);
		glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesPosition);
		glVertexAttribPointer(shd->Position, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shd->Color);
		glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesColor);
		glVertexAttribPointer(shd->Color, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		int qty; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &qty);
		glDrawElements(GL_TRIANGLES, qty/sizeof(GLuint), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(shd->Position);
		glDisableVertexAttribArray(shd->Color);
	}

	void Window()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		auto shd = Global->Get<StorageShader>("shader");
		
		wnd->setVerticalSyncEnabled(true);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glUseProgram(shd->Program);

		Perspective();
	}

	void Perspective()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;

		Perspective((Vector2i)wnd->getSize());
	}

	void Perspective(Vector2i Size)
	{
		auto shd = Global->Get<StorageShader>("shader");

		glViewport(0, 0, Size.x, Size.y);

		mat4 Projection = perspective(45.0f, (float)Size.x / (float)Size.y, 1.0f, 100.0f);
		glUniformMatrix4fv(shd->Projection, 1, GL_FALSE, value_ptr(Projection));
	}

	void Shader(string PathVertex, string PathFragment)
	{
		auto shd = Global->Add<StorageShader>("shader");

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