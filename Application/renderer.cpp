#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"

#include <unordered_map>
using namespace std;
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "settings.h"
#include "form.h"
#include "transform.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
#include "framebuffer.h"
#include "texture.h"
#include "material.h"


class ModuleRenderer : public Module
{
	void Init()
	{
		Opengl::InitGlew();

		Listeners();
	}

	void Update()
	{
		auto fbs = Entity->Get<StorageFramebuffer>();
		Vector2u size = Global->Get<RenderWindow>("window")->getSize();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uint n = 0;
		for(auto i : fbs)
		{
			auto fbr = i.second;
			auto shd = Entity->Get<StorageShader>(i.first);
			if(fbr->Id && shd->Program)
			{
				glViewport(0, 0, (int)(size.x * fbr->Size), (int)(size.y * fbr->Size));

				if(n < fbs.size() - 1)
					glBindFramebuffer(GL_FRAMEBUFFER, fbr->Id);
				else
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

				if(n > 0)
					Quad(shd->Program, shd->Samplers);
				else
					Forms(shd->Program);
			}
			n++;
		}

		Opengl::Test();
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto stg = Global->Get<StorageSettings>("settings");
			switch(Code)
			{
			case Keyboard::Key::F3:
				stg->Wireframe = !stg->Wireframe;
				break;
			}
		});
	}

	void Quad(GLuint Shader, unordered_map<string, GLuint> Samplers)
	{
		glUseProgram(Shader);
		glClear(GL_COLOR_BUFFER_BIT);

		int n = 0; for(auto i : Samplers)
		{
			glActiveTexture(GL_TEXTURE0 + n);
			glBindTexture(GL_TEXTURE_2D, i.second);
			glUniform1i(glGetUniformLocation(Shader, i.first.c_str()), n);
			n++;
		}

		glBegin(GL_QUADS);
			glVertex2i(0, 0);
			glVertex2i(1, 0);
			glVertex2i(1, 1);
			glVertex2i(0, 1);
		glEnd();

		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);		
		glUseProgram(0);
	}

	void Forms(GLuint Shader)
	{	
		auto stg = Global->Get<StorageSettings>("settings");
		auto fms = Entity->Get<StorageForm>();

		glUseProgram(Shader);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUniformMatrix4fv(glGetUniformLocation(Shader, "view"), 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		for(auto i : fms)
		{
			auto frm = i.second;
			auto tsf = Entity->Get<StorageTransform>(i.first);

			auto mat = Entity->Get<StorageMaterial>(frm->Material);
			if(!mat->Diffuse) continue;

			glUniformMatrix4fv(glGetUniformLocation(Shader, "model"), 1, GL_FALSE, value_ptr(tsf->Matrix));

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glBindTexture(GL_TEXTURE_2D, Entity->Get<StorageTexture>(mat->Diffuse)->Id);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);

			GLint size = 0;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
		}

		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
};
