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
#include "camera.h"


class ComponentRenderer : public Component
{
	void Init()
	{
		glewExperimental = GL_TRUE;
		int result = glewInit();
		Debug::PassFail("Glew initialization", result ? false : true);
		
		Shader("shaders/basic.vert", "shaders/basic.frag");

		Window();

		Listeners();
	}

	void Update()
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto cam = Global->Get<StorageCamera>("camera");
		auto fms = Entity->Get<StorageForm>();

		glClearColor(.4f,.6f,.9f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(shd->UniView, 1, GL_FALSE, value_ptr(cam->View));

		for(auto i = fms.begin(); i != fms.end(); ++i) Draw(i->first);
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto stg = Global->Get<StorageSettings>("settings");
			switch(Code)
			{
			case Keyboard::Key::F2:
				Wireframe(!stg->Wireframe);
				break;
			case Keyboard::Key::F3:
				stg->Verticalsync = !stg->Verticalsync;
				auto wnd = &Global->Get<StorageWindow>("window")->Window;
				wnd->setVerticalSyncEnabled(stg->Verticalsync);
			}
		});

		Event->Listen("WindowRecreated", [=]{
			Window();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Perspective(Size);
		});
	}

	void Draw(unsigned int id)
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto frm = Entity->Get<StorageForm>(id);
		auto tsf = Entity->Get<StorageTransform>(id);

		glUseProgram(frm->Program);

		mat4 Scale      = scale(mat4(1), frm->Scale);
		mat4 Translate  = translate(mat4(1), tsf->Position);
		mat4 Rotate     = rotate(mat4(1), tsf->Rotation.x, vec3(1, 0 ,0))
		                * rotate(mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
		                * rotate(mat4(1), tsf->Rotation.z, vec3(0, 0, 1));
		mat4 Vertex = Translate * Rotate * Scale;
		glUniformMatrix4fv(shd->UniVertex, 1, GL_FALSE, value_ptr(Vertex));

		mat4 Normal = Rotate;
		glUniformMatrix4fv(shd->UniNormal, 1, GL_FALSE, value_ptr(Normal));

		glEnableVertexAttribArray(shd->AtrVertex);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glVertexAttribPointer(shd->AtrVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shd->AtrNormal);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glVertexAttribPointer(shd->AtrNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shd->AtrTexcoord);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glVertexAttribPointer(shd->AtrTexcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);

		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		//glUniform1i(shd->Texture, 0);

		int count; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
		glDrawElements(GL_TRIANGLES, count/sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}

	void Window()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		auto stg = Global->Get<StorageSettings>("settings");
		auto shd = Global->Get<StorageShader>("shader");
		
		wnd->setVerticalSyncEnabled(stg->Verticalsync);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		
		glUseProgram(shd->Program);

		Perspective();
		Wireframe();
	}

	void Perspective()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;

		Perspective(wnd->getSize());
	}

	void Perspective(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto shd = Global->Get<StorageShader>("shader");

		glViewport(0, 0, Size.x, Size.y);

		mat4 Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
		glUniformMatrix4fv(shd->UniProjection, 1, GL_FALSE, value_ptr(Projection));
	}

	void Wireframe()
	{
		auto stg = Global->Get<StorageSettings>("settings");
		Wireframe(stg->Wireframe);
	}

	void Wireframe(bool State)
	{
		auto stg = Global->Get<StorageSettings>("settings");
		stg->Wireframe = State;
		glPolygonMode(GL_FRONT_AND_BACK, stg->Wireframe ? GL_LINE : GL_FILL);
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