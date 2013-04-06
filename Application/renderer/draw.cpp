#pragma once

#include "module.h"

using namespace std;
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "light.h"


void ModuleRenderer::Quad(GLuint Shader, unordered_map<string, GLuint> Samplers)
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

void ModuleRenderer::Forms(GLuint Shader, unordered_map<string, GLuint> Samplers)
{	
	auto stg = Global->Get<StorageSettings>("settings");
	auto fms = Entity->Get<StorageModel>();

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
		if(!frm->Elements) continue;
		if(!frm->Diffuse) continue;
		if(!Entity->Check<StorageTransform>(i.first)) continue;
		auto tsf = Entity->Get<StorageTransform>(i.first);

		glUniformMatrix4fv(glGetUniformLocation(Shader, "model"), 1, GL_FALSE, value_ptr(tsf->Matrix));

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindTexture(GL_TEXTURE_2D, frm->Diffuse);

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

void ModuleRenderer::Light(GLuint Shader, unordered_map<string, GLuint> Samplers)
{
	auto lis = Entity->Get<StorageLight>();

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(Shader);
	int n = 0; for(auto j : Samplers)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, j.second);
		glUniform1i(glGetUniformLocation(Shader, j.first.c_str()), n);
		n++;
	}

	mat4 view = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->View;

	for(auto i : lis)
	{
		int type = i.second->Type == StorageLight::DIRECTIONAL ? 0 : 1;
		vec3 pos = vec3(view * vec4(Entity->Get<StorageTransform>(i.first)->Position, !type ? 0 : 1));
		glUniform1i(glGetUniformLocation(Shader, "type"),      type);
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