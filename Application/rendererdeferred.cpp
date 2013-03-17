#pragma once

#include "system.h"
#include "debug.h"
#include "shaders.h"
#include "opengl.h"

#include <vector>
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


class ModuleRendererDeferred : public Module
{
	struct Texture { GLuint Id; GLenum Type, InternalType, Format; };
	struct Pass { GLuint Framebuffer; GLuint Program; unordered_map<string, GLuint> Textures; bool Active; };

	void Init()
	{
		Opengl::InitGlew();

		Pipeline();

		Resize();

		Listeners();
	}

	void Update()
	{
		uint i = 0;
		Pass pass;

		pass = Passes[i++].second;
		glBindFramebuffer(GL_FRAMEBUFFER, pass.Framebuffer);
		Forms(pass.Program);

		while(i < Passes.size() - 1)
		{
			pass = Passes[i++].second;
			if(pass.Active)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pass.Framebuffer);
				Quad(pass.Program, pass.Textures);
			}
		}

		pass = Passes[i++].second;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Quad(pass.Program, pass.Textures);

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
			case Keyboard::Key::F4:
				get_pass("antialiasing")->Active = !get_pass("antialiasing")->Active;
				break;
			}
		});

		Event->Listen("WindowRecreated", [=]{
			Passes.clear();
			Pipeline();

			Resize();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Resize(Size);
		});
	}

	void Pipeline()
	{
		get_or_create_texture("position", GL_RGBA32F);
		get_or_create_texture("normal",   GL_RGBA32F);
		get_or_create_texture("albedo",   GL_RGBA32F);
		get_or_create_texture("depth",    GL_DEPTH_COMPONENT32F);
		vector<string> forms_targets;
		forms_targets.push_back("position");
		forms_targets.push_back("normal");
		forms_targets.push_back("albedo");
		forms_targets.push_back("depth");
		create_pass("forms", make_pair("shaders/forms.vert", "shaders/forms.frag"), forms_targets);

		get_or_create_texture("light", GL_RGBA32F);
		unordered_map<string, string> light_uniforms;
		light_uniforms.insert(make_pair("position_tex", "position"));
		light_uniforms.insert(make_pair("normal_tex",   "normal"));
		light_uniforms.insert(make_pair("albedo_tex",   "albedo"));
		create_pass("light", "shaders/light.frag", "light", light_uniforms);

		get_or_create_texture("antialiasing", GL_RGBA32F);
		unordered_map<string, string> fxaa_uniforms;
		fxaa_uniforms.insert(make_pair("image_tex",    "light"));
		fxaa_uniforms.insert(make_pair("position_tex", "position"));
		fxaa_uniforms.insert(make_pair("normal_tex",   "normal"));
		create_pass("antialiasing", "shaders/antialiasing.frag", "antialiasing", fxaa_uniforms);

		get_or_create_texture("screen", GL_RGBA32F);
		unordered_map<string, string> screen_uniforms;
		screen_uniforms.insert(make_pair("image_tex", "antialiasing"));
		create_pass("screen", "shaders/screen.frag", "screen", screen_uniforms);
	}

	void Resize()
	{
		Resize(Global->Get<RenderWindow>("window")->getSize());
	}
	void Resize(Vector2u Size)
	{
		glViewport(0, 0, Size.x, Size.y);

		Shaders::Uniform(get_pass("forms")->Program, "projection", Global->Get<StorageCamera>("camera")->Projection);
		Shaders::Uniform(get_pass("antialiasing")->Program, "frameBufSize", vec2(Size.x, Size.y));

		for(auto i : Textures)
			resize_texture(i.first, Size);
	}

	////////////////////////////////////////////////////////////
	// Passes Functions
	////////////////////////////////////////////////////////////

	unordered_map<string, Texture> Textures;
	Texture get_texture(string name)
	{
		auto i = Textures.find(name);
		if(i != Textures.end())
			return i->second;
		throw std::out_of_range("There is no texture with the name (" + name + ").");
	}
	Texture get_or_create_texture(string name, GLenum internal_type)
	{
		auto i = Textures.find(name);
		if(i != Textures.end())
			return i->second;
		return create_texture(name, internal_type);
	}
	Texture create_texture(string name, GLenum internal_type)
	{
		auto i = Textures.find(name);
		if(i != Textures.end())
			throw std::logic_error("There is already a texture with the name (" + name + ").");

		Texture texture;
		glGenTextures(1, &texture.Id);

		texture.InternalType = internal_type;
		switch (internal_type)
		{
		case GL_RGBA16:
		case GL_RGBA16F:
		case GL_RGBA32F:
			texture.Type = GL_RGBA;
			break;
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32:
		case GL_DEPTH_COMPONENT32F:
			texture.Type = GL_DEPTH_COMPONENT;
		}
		switch (internal_type)
		{
		case GL_RGBA16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32:
			texture.Format = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_DEPTH_COMPONENT32F:
			texture.Format = GL_FLOAT;
			break;
		}

		Textures.insert(make_pair(name, texture));
		resize_texture(name);
		return texture;
	}
	void resize_texture(string name)
	{
		resize_texture(name, Global->Get<RenderWindow>("window")->getSize());
	}
	void resize_texture(string name, Vector2u size)
	{
		Texture texture = get_texture(name);
		glBindTexture(GL_TEXTURE_2D, texture.Id);
		glTexImage2D(GL_TEXTURE_2D, 0, texture.InternalType, size.x, size.y, 0, texture.Type, texture.Format, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint create_framebuffer(vector<string> targets)
	{
		GLuint id;
		glGenFramebuffers(1, &id);
		setup_framebuffer(id, targets);
		return id;
	}
	void setup_framebuffer(GLuint id, vector<string> targets)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		vector<GLenum> buffers;
		for(auto i : targets)
		{
			int n = buffers.size();
			Texture texture = get_texture(i);
			GLenum attachment;
			switch(texture.Type)
			{
			case GL_RGBA:
				attachment = GL_COLOR_ATTACHMENT0 + n;
				buffers.push_back(attachment);
				break;
			case GL_DEPTH_COMPONENT:
				attachment = GL_DEPTH_ATTACHMENT;
				break;
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.Id, 0);
		}
		glDrawBuffers(buffers.size(), &buffers[0]);

		Debug::PassFail("Renderer framebuffer setup", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	vector<pair<string, Pass>> Passes;
	Pass* get_pass(string name)
	{
		for(auto i = Passes.begin(); i != Passes.end(); ++i)
			if(i->first == name)
				return &i->second;
		throw std::out_of_range("There is no pass with the name (" + name + ").");
	}
	Pass* get_pass(uint index)
	{
		if(index < Passes.size())
			return &Passes[index].second;
		throw std::out_of_range("There is no pass with the index (" + to_string(index) + ").");
	}
	Pass create_pass(string name, string fragmentpath, string target, pair<string, string> texture)
	{
		unordered_map<string, string> textures;
		textures.insert(texture);
		return create_pass(name, fragmentpath, target, textures);
	}
	Pass create_pass(string name, string fragmentpath, string target, unordered_map<string, string> textures, unordered_map<string, void*> values = unordered_map<string, void*>())
	{
		vector<string> targets; targets.push_back(target);
		pair<string, string> shaderpaths("shaders/quad.vert", fragmentpath);
		return create_pass(name, shaderpaths, targets, textures);
	}
	Pass create_pass(string name, pair<string, string> shaderpaths, vector<string> targets, unordered_map<string, string> textures = unordered_map<string, string>()/*, unordered_map<string, void*> values = unordered_map<string, void*>()*/)
	{
		Pass pass;
		pass.Program = Shaders::Create(shaderpaths.first, shaderpaths.second);
		pass.Framebuffer = create_framebuffer(targets);
		for(auto i : textures)
			pass.Textures.insert(make_pair(i.first, get_texture(i.second).Id));
		pass.Active = true;
		Passes.push_back(make_pair(name, pass));
		return pass;
	}

	////////////////////////////////////////////////////////////
	// Draw Functions
	////////////////////////////////////////////////////////////

	void Quad(GLuint shader, unordered_map<string, GLuint> uniforms)
	{
		glUseProgram(shader);
		glClear(GL_COLOR_BUFFER_BIT);

		int n = 0; for(auto i : uniforms)
		{
			glActiveTexture(GL_TEXTURE0 + n);
			glBindTexture(GL_TEXTURE_2D, i.second);
			glUniform1i(glGetUniformLocation(shader, i.first.c_str()), n);
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

	void Forms(GLuint shader)
	{	
		auto stg = Global->Get<StorageSettings>("settings");
		auto fms = Entity->Get<StorageForm>();

		glUseProgram(shader);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		for(auto i : fms)
		{
			auto frm = Entity->Get<StorageForm>(i.first);
			auto tsf = Entity->Get<StorageTransform>(i.first);

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, value_ptr(tsf->Matrix));

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