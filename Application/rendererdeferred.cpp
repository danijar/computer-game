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


class ComponentRendererDeferred : public Component
{
	unordered_map<string, pair<GLuint, pair<GLenum, GLenum>>> Textures;             // name, texture id, type, internal type
	vector<pair<GLuint, pair<GLuint, unordered_map<string, GLuint>>>> Framebuffers; // framebuffer id, shader id, uniform name, uniform id

	GLuint shaderforms;

	void Init()
	{
		Opengl::InitGlew(); 

		Pipeline();

		Resize();
		
		Listeners();
	}

	void Update()
	{
		unsigned int i = 0;

		// forms
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers[i].first);
		Forms(Framebuffers[i].second.first, Framebuffers[i].second.second);
		i++;

		// deferred
		while(i < Framebuffers.size() - 1)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers[i].first);
			Quad(Framebuffers[i].second.first, Framebuffers[i].second.second);
			i++;
		}

		// screen
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		Quad(Framebuffers[i].second.first, Framebuffers[i].second.second);
		i++;
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

	void Resize(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");

		glClearColor(.4f,.6f,.9f,0.f);
		glViewport(0, 0, Size.x, Size.y);

		glUseProgram(shaderforms); // necessary?
		mat4 Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
		glUniformMatrix4fv(glGetUniformLocation(shaderforms, "projection"), 1, GL_FALSE, value_ptr(Projection));

		for(auto i : Textures)
		{
			resizeTexture(i.second.first, i.second.second, Size);
		}
	}

	void Pipeline()
	{
		unordered_map<string, pair<GLenum, GLenum>> gbuffer;
		gbuffer.insert(make_pair("position", make_pair(GL_RGBA32F, GL_RGBA)));
		gbuffer.insert(make_pair("normal",   make_pair(GL_RGBA32F, GL_RGBA)));
		gbuffer.insert(make_pair("albedo",   make_pair(GL_RGBA32F, GL_RGBA)));
		gbuffer.insert(make_pair("depth",    make_pair(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)));
		auto framebuffer = createFramebuffer(make_pair("shaders/basic.vert", "shaders/basic.frag"), gbuffer);
		shaderforms = framebuffer.second;

		unordered_map<string, GLuint> uniforms;
		uniforms.insert(make_pair("position_tex", Textures["position"].second.first));
		uniforms.insert(make_pair("normal_tex",   Textures["normal"].second.first));
		uniforms.insert(make_pair("albedo_tex",   Textures["albedo"].second.first));
		createFramebuffer(make_pair("shaders/quad.vert", "shaders/light.frag"), "result", uniforms);
	}

	GLuint createTexture(string Name, pair<GLenum, GLenum> Type)
	{
		GLuint Id;
		auto i = Textures.find(Name);
		if(i != Textures.end())
		{
			Id = i->second.first;
		}
		else
		{
			glGenTextures(1, &Id);
			glBindTexture(GL_TEXTURE_2D, Id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			resizeTexture(Id, Type, Global->Get<RenderWindow>("window")->getSize());

			Textures.insert(make_pair(Name, make_pair(Id, Type)));
		}
		return Id;
	}

	void resizeTexture(GLuint Id, pair<GLenum, GLenum> Type, Vector2u Size)
	{
		glBindTexture(GL_TEXTURE_2D, Id);
		glTexImage2D(GL_TEXTURE_2D, 0, Type.first, Size.x, Size.y, 0, Type.second, GL_FLOAT, NULL); // take care of depth textures
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	pair<GLuint, GLuint> createFramebuffer(pair<string, string> Shaderpath, string Target, unordered_map<string, GLuint> Uniforms)
	{
		unordered_map<string, pair<GLenum, GLenum>> Targets;
		Targets.insert(make_pair(Target, make_pair(GL_RGBA32F, GL_RGBA)));
		return createFramebuffer(Shaderpath, Targets, Uniforms);
	}

	pair<GLuint, GLuint> createFramebuffer(pair<string, string> Shaderpath, unordered_map<string, pair<GLenum, GLenum>> Targets, unordered_map<string, GLuint> Uniforms = unordered_map<string, GLuint>())
	{
		GLuint Program = Shaders::Create(Shaderpath.first, Shaderpath.second);

		GLuint Id;
		glGenFramebuffers(1, &Id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Id);

		vector<GLenum> buffers;
		int n = 0; for(auto i : Targets)
		{
			switch(i.second.second)
			{
			case GL_RGBA:
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + n, GL_TEXTURE_2D, createTexture(i.first, i.second), 0);
				glBindFragDataLocation(Program, n, i.first.c_str());
				buffers.push_back(GL_COLOR_ATTACHMENT0 + n);
				n++;
				break;
			case GL_DEPTH_COMPONENT:
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, createTexture(i.first, i.second), 0);
				break;
			}
		}
		glDrawBuffers(buffers.size(), &buffers[0]);
		glLinkProgram(Program);

		Debug::PassFail("Renderer framebuffer creation", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		Framebuffers.push_back(make_pair(Id, make_pair(Program, Uniforms)));
		return make_pair(Id, Program);
	}

	void Forms(GLuint Shader, unordered_map<string, GLuint> Uniforms)
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto fms = Entity->Get<StorageForm>();

		if(Uniforms.size() > 0) Debug::Fail("Uniforms of the geometry pass are ignored.");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(Shader);
		
		GLuint model   = glGetUniformLocation(Shader, "model"),
			   view    = glGetUniformLocation(Shader, "view" ),
			   texture = glGetUniformLocation(Shader, "tex"  );

		glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		/*
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		*/

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

		Opengl::Test();
	}

	void Quad(GLuint Shader, unordered_map<string, GLuint> Uniforms)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(Shader);

		int n = 0; for(auto i : Uniforms)
		{
			glActiveTexture(GL_TEXTURE0 + n);
			glBindTexture(GL_TEXTURE_2D, i.second);
			glUniform1i(glGetUniformLocation(Shader, i.first.c_str()), n);
			n++;
		}

		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glVertex2f(1.f, 0.f);
			glVertex2f(1.f, 1.f);
			glVertex2f(0.f, 1.f);
		glEnd();

		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);		
		glUseProgram(0);
	}
};
