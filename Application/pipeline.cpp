#pragma once

#include "system.h"
#include "opengl.h"

#include <unordered_map>
using namespace std;
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "shader.h"
#include "framebuffer.h"
#include "camera.h"


class ModulePipeline : public Module
{
	unordered_map<string, GLuint> Textures;

	GLuint forms, antialiasing;

	void Init()
	{
		Opengl::InitGlew();

		Pipeline();

		Uniforms();

		Listeners();
	}

	void Update()
	{

	}

	void Listeners()
	{
		Event->Listen("WindowRecreated", [=]{
			Uniforms();
		});

		Event->Listen("WindowResize", [=]{
			Uniforms();
		});

		Event->Listen("ShaderUpdated", [=]{
			Uniforms();
		});
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
};
