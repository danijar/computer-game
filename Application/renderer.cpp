#pragma once

#include "system.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;
#include <iostream>
#include <sstream>
#include <fstream>
#include <streambuf>
using namespace std;

#include "window.h"
#include "settings.h"
#include "buffers.h"


class ComponentRenderer : public Component
{

	void Init()
	{
		auto stg = Storage->Get<StorageSettings>("settings");
		auto bfs = Storage->Add<StorageBuffers>("buffers");

		glewExperimental = GL_TRUE;
		glewInit();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glGenVertexArrays(1, &bfs->VertexArray);
		glBindVertexArray(bfs->VertexArray);
		glGenBuffers(1, &bfs->VertexBuffer);
		glGenBuffers(1, &bfs->ElementBuffer);

		ShaderProgram = ProgramCreate("shaders/vertex.txt", "", "shaders/fragment.txt");

		Setup();

		Listeners();

		Cube();

		// jumping
		jumping = false;
	}

	void Update()
	{
		auto stg = Storage->Get<StorageSettings>("settings");

		float time = clock.getElapsedTime().asSeconds();

		// clear
		glClearColor(.4f,.6f,.9f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// model matrix
		mat4 ModelMatrix;

		const float sle = .25f;
		const float rdn = 2.5f;
		ModelMatrix = scale(ModelMatrix, vec3(sle, sle, sle));								// scale down a bit
		ModelMatrix = translate(ModelMatrix, vec3(-1.5f, -1.5f, 0.f));						// move away from camera a bit
		ModelMatrix = translate(ModelMatrix, vec3(sin(time) * rdn, cos(time) * rdn, 0));	// turn around a radian
		ModelMatrix = rotate(ModelMatrix, time * 360.f, vec3(0, 0, 1));						// spin around itself

		// jumping
		if(jumping){
			float jumpposition = glm::sin((float)jumptime.getElapsedTime().asSeconds() * 10);
			if(jumpposition < 0) jumping = false;
			else ModelMatrix = translate(ModelMatrix, vec3(0, 0, jumpposition));
		}

		GLint ModelUniform = glGetUniformLocation(ShaderProgram, "model");
		glUniformMatrix4fv(ModelUniform, 1, GL_FALSE, value_ptr(ModelMatrix));

		// view matrix
		mat4 ViewMatrix = lookAt(
			vec3(1.2, 1.2, 1.2),
			vec3(0.0, 0.0, 0.0),
			vec3(0.0, 0.0, 1.0)
		);

		GLint ViewUniform = glGetUniformLocation(ShaderProgram, "view");
		glUniformMatrix4fv(ViewUniform, 1, GL_FALSE, value_ptr(ViewMatrix));

		// projection matrix
		mat4 ProjectionMatrix = perspective(45.0f, stg->AspectRatio(), 1.0f, 10.0f);

		GLint ProjectionUniform = glGetUniformLocation(ShaderProgram, "proj" );
		glUniformMatrix4fv(ProjectionUniform, 1, GL_FALSE, value_ptr(ProjectionMatrix));

		// draw
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// swap buffers
		Storage->Get<StorageWindow>("window")->Window.display();
	}

	GLuint ShaderProgram;
	Clock clock;

	// jumping
	bool jumping;
	Clock jumptime;

	void Listeners()
	{
		Event->Listen("WindowCreated", [=]{
			Setup();
		});

		Event->ListenData("WindowResize", [=](void* Size){
			auto sze = *(Event::SizeEvent*)Size;
			glViewport(0, 0, sze.width, sze.height);
		});

		Event->Listen("InputBindJump", [=]{
			if(!jumping)
			{
				jumping = true;
				jumptime.restart();
			}
		});
	}

	void Setup()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;

		wnd->setVerticalSyncEnabled(true);
		glEnable(GL_DEPTH_TEST);
		ProgramActivate(ShaderProgram);
	}

	void Cube()
	{
		auto bfs = Storage->Get<StorageBuffers>("buffers");

		// vertices
		const float Vertices[] = {
  			-1.f, -1.f,  1.f,  1.f,  0.f,  0.f,  .8f,
			 1.f, -1.f,  1.f,  0.f,  1.f,  0.f,  .8f,
			 1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  .8f,
			-1.f,  1.f,  1.f,  1.f,  1.f,  1.f,  .8f,
   
			-1.f, -1.f, -1.f,  0.f,  0.f,  1.f,  .8f,
			 1.f, -1.f, -1.f,  1.f,  1.f,  1.f,  .8f,
			 1.f,  1.f, -1.f,  1.f,  0.f,  0.f,  .8f,
			-1.f,  1.f, -1.f,  0.f,  1.f,  0.f,  .8f,
		};

		glBindBuffer(GL_ARRAY_BUFFER, bfs->VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

		// elements
		const int Elements[] = {
			0, 1, 2, 2, 3, 0,
			1, 5, 6, 6, 2, 1,
			7, 6, 5, 5, 4, 7,
			4, 0, 3, 3, 7, 4,
			4, 5, 1, 1, 0, 4,
			3, 2, 6, 6, 7, 3,
		};

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bfs->ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), &Elements, GL_STATIC_DRAW);

		// position
		GLint PositionAttribute = glGetAttribLocation(ShaderProgram, "position");
		glEnableVertexAttribArray(PositionAttribute);
		glVertexAttribPointer(PositionAttribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

		// color
		GLint ColorAttribute = glGetAttribLocation(ShaderProgram, "color");
		glEnableVertexAttribArray(ColorAttribute);
		glVertexAttribPointer(ColorAttribute, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	}



	// #region: Shader and Programs

	GLuint ProgramCreate(string PathVertex, string PathGeometry, string PathFragment)
	{
		int Id = glCreateProgram();

		int Shader[3];
		if(PathVertex   != ""){ Shader[0] = ShaderCreate(PathVertex,   GL_VERTEX_SHADER  ); glAttachShader(Id, Shader[0]); }
		if(PathGeometry != ""){ Shader[1] = ShaderCreate(PathGeometry, GL_GEOMETRY_SHADER); glAttachShader(Id, Shader[1]); }
		if(PathFragment != ""){ Shader[2] = ShaderCreate(PathFragment, GL_FRAGMENT_SHADER); glAttachShader(Id, Shader[2]); }

		glLinkProgram(Id);

		if(PathVertex   != "") ShaderDelete(Shader[0]);
		if(PathGeometry != "") ShaderDelete(Shader[1]);
		if(PathFragment != "") ShaderDelete(Shader[2]);

		cout << "Shader program create " << Id << endl;
		return Id;
	}

	void ProgramActivate(int Id)
	{
		glUseProgram(Id);
		cout << "Shader program activate " << Id << endl;
	}

	void ProgramDelete(int Id)
	{
		glDeleteProgram(Id);
		cout << "Shader program delete " << Id << endl;
	}

	bool ProgramTest(int Id)
	{
		// link result
		GLint Success;
		glGetProgramiv(Id, GL_LINK_STATUS, &Success);
		bool Result = (Success == GL_TRUE) ? true : false;
		cout << "Shader program link " << Id << " " << (Result ? "success" : "fail") << endl;

		// info log
		GLchar Log[513];
		GLsizei Length;
		glGetProgramInfoLog(Id, 512, &Length, Log);
		if(Length > 0) cout << Log << endl;

		return Result;
	}

	GLuint ShaderCreate(string Path, int Type)
	{
		string Source;
		ifstream Stream(Path);

		if(!Stream.is_open()) cout << "Shader create fail " << Path << endl;

		// load
		Stream.seekg(0, ios::end);   
		Source.reserve(static_cast<unsigned int>(Stream.tellg()));
		Stream.seekg(0, ios::beg);
		Source.assign((istreambuf_iterator<char>(Stream)), istreambuf_iterator<char>());

		// compile
		const GLchar* SourceString = Source.c_str();
		int Id = glCreateShader(Type);
		glShaderSource(Id, 1, &SourceString, NULL);
		glCompileShader(Id);

		cout << "Shader create " << Id << endl;
		return Id;
	}

	void ShaderDelete(int Id)
	{
		glDeleteShader(Id);
		cout << "Shader delete " << Id << endl;
	}

	bool ShaderTest(int Id)
	{
		// compiler result
		GLint Success;
		glGetShaderiv(Id, GL_COMPILE_STATUS, &Success);
		bool Result = (Success == GL_TRUE) ? true : false;
		cout << "Shader compile " << Id << " " << (Result ? "success" : "fail") << endl;

		// info log
		GLchar Log[513];
		GLsizei Length;
		glGetShaderInfoLog(Id, 512, &Length, Log);
		if(Length > 0) cout << Log << endl;

		return Result;
	}

	// #endregion: Shader and Programs

};