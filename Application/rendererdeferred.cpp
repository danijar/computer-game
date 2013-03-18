#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"

#include <unordered_map>
#include <fstream>
using namespace std;
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "settings.h"
#include "form.h"
#include "transform.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
#include "framebuffer.h"


class ModuleRendererDeferred : public Module
{
	unordered_map<string, GLuint> Textures;
	GLuint forms, antialiasing;

	void Init()
	{
		Opengl::InitGlew();

		Pipeline();

		Resize();

		Listeners();
	}

	void Update()
	{
		auto fbs = Entity->Get<StorageFramebuffer>();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		uint n = 0;
		for(auto i : fbs)
		{
			auto fbr = i.second;
			auto shd = Entity->Get<StorageShader>(i.first);
			if(fbr->Id && shd->Program)
			{
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

		Event->Listen("WindowRecreated", [=]{
			Resize();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Resize(Size);
		});

		Event->Listen("ShaderUpdated", [=]{
			Uniforms();
		});
	}

	void Resize()
	{
		Resize(Global->Get<RenderWindow>("window")->getSize());
	}
	void Resize(Vector2u Size)
	{
		glViewport(0, 0, Size.x, Size.y);
		Uniforms();
	}

	void Pipeline()
	{
		unordered_map<GLenum, pair<string, GLenum>> forms_targets;
		forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT0, make_pair("position", GL_RGBA32F)));
		forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT1, make_pair("normal",   GL_RGBA32F)));
		forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT2, make_pair("albedo",   GL_RGBA32F)));
		forms_targets.insert(make_pair(GL_DEPTH_ATTACHMENT,  make_pair("depth",    GL_DEPTH_COMPONENT32F)));
		forms = CreatePass("forms.vert", "forms.frag", forms_targets);

		unordered_map<string, string> light_samplers;
		light_samplers.insert(make_pair("position_tex", "position"));
		light_samplers.insert(make_pair("normal_tex",   "normal"));
		light_samplers.insert(make_pair("albedo_tex",   "albedo"));
		CreatePass("light.frag", "light", light_samplers);

		unordered_map<string, string> antialiasing_samplers;
		antialiasing_samplers.insert(make_pair("image_tex",    "light"));
		antialiasing_samplers.insert(make_pair("position_tex", "position"));
		antialiasing_samplers.insert(make_pair("normal_tex",   "normal"));
		antialiasing = CreatePass("antialiasing.frag", "antialiasing", antialiasing_samplers);

		CreatePass("screen.frag", "screen", make_pair("image_tex", "antialiasing"));
	}

	void Uniforms()
	{
		if(GLuint id = Entity->Get<StorageShader>(forms)->Program)
		{
			glUseProgram(id);
			glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->Projection));
			glUseProgram(0);
		}

		if(GLuint id = Entity->Get<StorageShader>(antialiasing)->Program)
		{
			Vector2u Size = Global->Get<RenderWindow>("window")->getSize();
			glUseProgram(id);
			glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));
			glUseProgram(0);
		}
	}

	////////////////////////////////////////////////////////////
	// Passes
	////////////////////////////////////////////////////////////

	unsigned int CreatePass(string PathFragment, string Target, pair<string, string> Sampler)
	{
		unordered_map<string, string> samplers;
		samplers.insert(Sampler);
		return CreatePass(PathFragment, Target, samplers);
	}

	unsigned int CreatePass(string PathFragment, string Target, unordered_map<string, string> Samplers = unordered_map<string, string>())
	{
		unordered_map<GLenum, pair<string, GLenum>> targets;
		targets.insert(make_pair(GL_COLOR_ATTACHMENT0, make_pair(Target, GL_RGBA32F)));
		return CreatePass("quad.vert", PathFragment, targets, Samplers);
	}

	unsigned int CreatePass(string PathVertex, string PathFragment, unordered_map<GLenum, pair<string, GLenum>> Targets, unordered_map<string, string> Samplers = unordered_map<string, string>())
	{
		unsigned int id = Entity->New();
		auto shd = Entity->Add<StorageShader>(id);
		auto frb = Entity->Add<StorageFramebuffer>(id);

		shd->PathVertex   = PathVertex;
		shd->PathFragment = PathFragment;
		
		for(auto i : Targets)
		{
			GLuint id = frb->AddTarget(i.first, i.second.second);
			Textures.insert(make_pair(i.second.first, id));
		}

		for(auto i : Samplers)
			shd->Samplers.insert(make_pair(i.first, Textures[i.second]));

		return id;
	}

	////////////////////////////////////////////////////////////
	// Draw Functions
	////////////////////////////////////////////////////////////

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
			auto frm = Entity->Get<StorageForm>(i.first);
			auto tsf = Entity->Get<StorageTransform>(i.first);

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

			glBindTexture(GL_TEXTURE_2D, frm->Texture);

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
