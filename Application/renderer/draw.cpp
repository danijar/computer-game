#include "module.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace sf;
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "form.h"
#include "model.h"
#include "light.h"


void ModuleRenderer::DrawQuad(Pass *Pass, bool Screen)
{
	if(Screen) glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else       glBindFramebuffer(GL_FRAMEBUFFER, Pass->Framebuffer);
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();
	glViewport(0, 0, (int)(size.x * Pass->Size), (int)(size.y * Pass->Size));
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(Pass->Shader);
	int n = 0; for(auto i : Pass->Samplers)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, i.second);
		glUniform1i(glGetUniformLocation(Pass->Shader, i.first.c_str()), n);
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

void ModuleRenderer::DrawQuadStenciled(Pass *Pass)
{
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_GREATER, 0, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	DrawQuad(Pass);

	glDisable(GL_STENCIL_TEST);
}

void ModuleRenderer::DrawForms(Pass *Pass)
{	
	auto stg = Global->Get<Settings>("settings");
	auto fms = Entity->Get<Model>();

	glBindFramebuffer(GL_FRAMEBUFFER, Pass->Framebuffer);
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();
	glViewport(0, 0, (int)(size.x * Pass->Size), (int)(size.y * Pass->Size));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
	glUseProgram(Pass->Shader);
	glUniformMatrix4fv(glGetUniformLocation(Pass->Shader, "view"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->View));

	glPolygonMode(GL_FRONT_AND_BACK, stg->Wireframe ? GL_LINE : GL_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	// fill stencil buffer with ones where geometry is
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);

	for(auto i : fms)
	{
		auto frm = i.second;

		// improve by using fallbacks instead of skipping
		if(!frm->Elements) continue;
		if(!frm->Diffuse) continue;
		if(!Entity->Check<Form>(i.first)) continue;
		auto tsf = Entity->Get<Form>(i.first);

		glUniformMatrix4fv(glGetUniformLocation(Pass->Shader, "model"), 1, GL_FALSE, value_ptr(tsf->Matrix()));

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
	//glDepthMask(GL_FALSE);

	glStencilFunc(GL_EQUAL, 0, 0xFF);
	glStencilMask(0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	DrawSky(Pass);

	glDisable(GL_STENCIL_TEST);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ModuleRenderer::DrawSky(Pass *Pass)
{	
	Model *frm = Global->Get<Model>("sky");
	if(frm->Elements && frm->Diffuse)
	{
		vec3 position = Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position();
		mat4 matrix = translate(mat4(1), position);

		glUniformMatrix4fv(glGetUniformLocation(Pass->Shader, "model"), 1, GL_FALSE, value_ptr(matrix));

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
}

void ModuleRenderer::DrawLight(Pass *Pass)
{
	auto lis = Entity->Get<Light>();

	glBindFramebuffer(GL_FRAMEBUFFER, Pass->Framebuffer);
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();
	glViewport(0, 0, (int)(size.x * Pass->Size), (int)(size.y * Pass->Size));
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(Pass->Shader);
	int n = 0; for(auto j : Pass->Samplers)
	{
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, j.second);
		glUniform1i(glGetUniformLocation(Pass->Shader, j.first.c_str()), n);
		n++;
	}

	mat4 view = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->View;

	for(auto i : lis)
	{
		int type = i.second->Type == Light::DIRECTIONAL ? 0 : 1;
		vec3 pos = vec3(view * vec4(Entity->Get<Form>(i.first)->Position(), !type ? 0 : 1));
		glUniform1i(glGetUniformLocation(Pass->Shader, "type"),      type);
		glUniform3f(glGetUniformLocation(Pass->Shader, "light"),     pos.x, pos.y, pos.z);
		glUniform3f(glGetUniformLocation(Pass->Shader, "color"),     i.second->Color.x, i.second->Color.y, i.second->Color.z);
		glUniform1f(glGetUniformLocation(Pass->Shader, "radius"),    i.second->Radius);
		glUniform1f(glGetUniformLocation(Pass->Shader, "intensity"), i.second->Intensity);

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
