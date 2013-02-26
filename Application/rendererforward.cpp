#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"
#include "shaders.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "settings.h"
#include "form.h"
#include "transform.h"
#include "camera.h"


class ComponentRendererForward : public Component
{
	GLuint shader;

	void Init()
	{
		Opengl::InitGlew(); 

		shader = Shaders::Create("shaders/basic.vert", "shaders/basic.frag");
		Resize();
		
		Listeners();

		Opengl::Test();
	}

	void Update()
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto fms = Entity->Get<StorageForm>();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		
		GLuint model   = glGetUniformLocation(shader, "model"),
			   view    = glGetUniformLocation(shader, "view" ),
			   texture = glGetUniformLocation(shader, "tex"  );

		glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		for(auto i : fms)
		{
			auto frm = Entity->Get<StorageForm>(i.first);
			auto tsf = Entity->Get<StorageTransform>(i.first);

			glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(tsf->Matrix));

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindTexture(GL_TEXTURE_2D, frm->Texture);
			glUniform1i(texture, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
			
			GLint size = 0;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);

			Opengl::Test();
		}

		glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto stg = Global->Get<StorageSettings>("settings");
			switch(Code)
			{
			case Keyboard::Key::F2:
				stg->Wireframe = !stg->Wireframe;
				break;
			}
		});

		Event->Listen("WindowRecreated", [=]{
			Resize();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Resize(Size);
		});
	}

	void Resize()
	{
		Resize(Global->Get<RenderWindow>("window")->getSize());
	}

	mat4 Projection;
	void Resize(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");

		glClearColor(.4f,.6f,.9f,0.f);

		glViewport(0, 0, Size.x, Size.y);

		glUseProgram(shader);
		Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, value_ptr(Projection));
	}
};