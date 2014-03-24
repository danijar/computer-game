#include "module.h"

#include <sfml/Graphics/RenderWindow.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "type/settings/type.h"
#include "type/camera/type.h"
#include "type/form/type.h"
#include "type/model/type.h"
#include "type/light/type.h"

using namespace sf;
using namespace glm;


void ModuleRenderer::DrawQuad(Pass *Pass)
{
	int n = 0;
	for(auto i : Pass->Samplers) {
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, i.second);
		glUniform1i(glGetUniformLocation(Pass->Program, i.first.c_str()), n);
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
}

void ModuleRenderer::DrawScreen(Pass *Pass)
{
	int n = 0;
	for (auto i : Pass->Samplers) {
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, i.second);
		glUniform1i(glGetUniformLocation(Pass->Program, i.first.c_str()), n);
		n++;
	}

	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(1, 0);
		glVertex2i(1, 1);
		glVertex2i(0, 1);
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer::DrawForms(Pass *Pass)
{	
	auto stg = Global->Get<Settings>("settings");
	auto mls = Entity->Get<Model>();
		
	glUniformMatrix4fv(glGetUniformLocation(Pass->Program, "view"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->View));

	glPolygonMode(GL_FRONT_AND_BACK, stg->Is("Wireframe") ? GL_LINE : GL_FILL);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(Pass->Program, "diffusemap"),  0);
	glUniform1i(glGetUniformLocation(Pass->Program, "normalmap"),   1);
	glUniform1i(glGetUniformLocation(Pass->Program, "specularmap"), 2);

	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (auto i : mls) {
		if (!Visible(i.first)) continue;

		auto mdl = i.second;

		// improve by using fallbacks instead of skipping
		if (!mdl->Elements) continue;
		if (!mdl->Diffuse) continue;
		if (!Entity->Check<Form>(i.first)) continue;
		auto frm = Entity->Get<Form>(i.first);

		glUniformMatrix4fv(glGetUniformLocation(Pass->Program, "model"), 1, GL_FALSE, value_ptr(frm->Matrix()));

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->Positions);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->Normals);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, mdl->Texcoords);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		glUniform1f(glGetUniformLocation(Pass->Program, "shininess"), mdl->Shininess);

		// diffuse map
		glActiveTexture(GL_TEXTURE0);
		if (mdl->Diffuse) {
			glBindTexture(GL_TEXTURE_2D, mdl->Diffuse);
		} // else fallback texture

		// normal map
		glActiveTexture(GL_TEXTURE1);
		if (mdl->Normal) {
			glBindTexture(GL_TEXTURE_2D, mdl->Normal);
			glUniform1i(glGetUniformLocation(Pass->Program, "hasnormalmap"), 1);
		} else {
			glUniform1i(glGetUniformLocation(Pass->Program, "hasnormalmap"), 0); // fallback texture instead
		}

		// specular map
		glActiveTexture(GL_TEXTURE2);
		if (mdl->Specular) {
			glBindTexture(GL_TEXTURE_2D, mdl->Specular);
			glUniform1i(glGetUniformLocation(Pass->Program, "hasspecularmap"), 1);
		} else {
			glUniform1i(glGetUniformLocation(Pass->Program, "hasspecularmap"), 0); // fallback texture instead
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->Elements);

		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0); // should I unbind all samplers?
}

void ModuleRenderer::DrawSky(Pass *Pass)
{	
	auto stg = Global->Get<Settings>("settings");
		
	glUniformMatrix4fv(glGetUniformLocation(Pass->Program, "view"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->View));

	glPolygonMode(GL_FRONT_AND_BACK, stg->Is("Wireframe") ? GL_LINE : GL_FILL);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Model *frm = Global->Get<Model>("sky");
	if (frm->Elements && frm->Diffuse) {
		vec3 position = Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Position();
		mat4 matrix = translate(mat4(1), position);

		glUniformMatrix4fv(glGetUniformLocation(Pass->Program, "model"), 1, GL_FALSE, value_ptr(matrix));

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer::DrawLights(Pass *Pass)
{
	auto lis = Entity->Get<Light>();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	int n = 0;
	for (auto i : Pass->Samplers) {
		glActiveTexture(GL_TEXTURE0 + n);
		glBindTexture(GL_TEXTURE_2D, i.second);
		glUniform1i(glGetUniformLocation(Pass->Program, i.first.c_str()), n);
		n++;
	}

	mat4 view = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->View;

	for (auto i : lis) {
		int type = i.second->Type == Light::DIRECTIONAL ? 0 : 1;
		vec3 pos = vec3(view * vec4(Entity->Get<Form>(i.first)->Position(), !type ? 0 : 1));
		glUniform1i(glGetUniformLocation(Pass->Program, "type"),      type);
		glUniform3f(glGetUniformLocation(Pass->Program, "light"),     pos.x, pos.y, pos.z);
		glUniform3f(glGetUniformLocation(Pass->Program, "color"),     i.second->Color.x, i.second->Color.y, i.second->Color.z);
		glUniform1f(glGetUniformLocation(Pass->Program, "radius"),    i.second->Radius);
		glUniform1f(glGetUniformLocation(Pass->Program, "intensity"), i.second->Intensity);

		glBegin(GL_QUADS);
			glVertex2i(0, 0);
			glVertex2i(1, 0);
			glVertex2i(1, 1);
			glVertex2i(0, 1);
		glEnd();
	}

	glDisable(GL_BLEND);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
