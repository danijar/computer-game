#pragma once

#include "system.h"

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
#include "framebuffer.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "form.h"
#include "transform.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "light.h"


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

				if(n == 0)
					Forms(shd->Program);
				else if(n == 1)
					Light(shd->Program, shd->Samplers);
				else
					Quad(shd->Program, shd->Samplers);
					
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
		//glClearColor(0.0, 0.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUniformMatrix4fv(glGetUniformLocation(Shader, "view"), 1, GL_FALSE, value_ptr(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		for(auto i : fms)
		{
			auto frm = i.second;

			// improve by using fallbacks instead of skipping
			if(!Entity->Check<StorageTransform>(i.first)) continue;
			if(!Entity->Check<StorageMesh>(frm->Mesh)) continue;
			if(!Entity->Check<StorageMaterial>(frm->Material)) continue;
			
			auto tsf = Entity->Get<StorageTransform>(i.first);
			auto msh = Entity->Get<StorageMesh>(frm->Mesh);
			auto mat = Entity->Get<StorageMaterial>(frm->Material);

			if(!mat->Diffuse) continue;

			glUniformMatrix4fv(glGetUniformLocation(Shader, "model"), 1, GL_FALSE, value_ptr(tsf->Matrix));

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, msh->Positions);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, msh->Normals);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, msh->Texcoords);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glBindTexture(GL_TEXTURE_2D, Entity->Get<StorageTexture>(mat->Diffuse)->Id);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->Elements);

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

	void Light(GLuint Shader, unordered_map<string, GLuint> Samplers)
	{
		auto lis = Entity->Get<StorageLight>();

		glUseProgram(Shader);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		int n = 0; for(auto i : Samplers)
		{
			glActiveTexture(GL_TEXTURE0 + n);
			glBindTexture(GL_TEXTURE_2D, i.second);
			glUniform1i(glGetUniformLocation(Shader, i.first.c_str()), n);
			n++;
		}

		mat4 view = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->View;

		for(auto i : lis)
		{
			vec3 pos = (view * vec4(Entity->Get<StorageTransform>(i.first)->Position, 1)).swizzle(X, Y, Z);
			glUniform3f(glGetUniformLocation(Shader, "light"),     pos.x, pos.y, pos.z);
			glUniform3f(glGetUniformLocation(Shader, "color"),     i.second->Color.x, i.second->Color.y, i.second->Color.z);
			glUniform1f(glGetUniformLocation(Shader, "radius"),    i.second->Radius);
			glUniform1f(glGetUniformLocation(Shader, "intensity"), i.second->Intensity);

			glBegin(GL_QUADS);
				glVertex2i(0, 0);
				glVertex2i(1, 0);
				glVertex2i(1, 1);
				glVertex2i(0, 1);
			glEnd();
		}

		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);		
		glUseProgram(0);
	}
};
