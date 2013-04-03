#pragma once

#include "system.h"

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
#include "texture.h"


class ModulePipeline : public Module
{
	unordered_map<string, GLuint> Textures;

	GLuint forms, occlusion, combine, antialiasing;

	void Init()
	{
		Opengl->Init();

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
		light_samplers.insert(make_pair("positions", "position"));
		light_samplers.insert(make_pair("normals",   "normal"));
		CreatePass("light.frag", "light", light_samplers);

		unordered_map<string, string> combine_samplers;
		combine_samplers.insert(make_pair("albedo", "albedo"));
		combine_samplers.insert(make_pair("lights", "light"));
		CreatePass("combine.frag", "image", combine_samplers);

		unordered_map<string, string> occlusion_samplers;
		occlusion_samplers.insert(make_pair("position_tex", "position"));
		occlusion_samplers.insert(make_pair("normal_tex",   "normal"));
		occlusion = CreatePass("occlusion.frag", "occlusion", occlusion_samplers, 0.5);
		unsigned int id = Entity->New();
		auto tex = Entity->Add<StorageTexture>(id);
		tex->Path = "noise.png";
		Entity->Get<StorageShader>(occlusion)->Samplers.insert(make_pair("noise_tex", tex->Id));

		unordered_map<string, string> blur_samplers;
		blur_samplers.insert(make_pair("image_tex",  "image"));
		blur_samplers.insert(make_pair("effect_tex", "occlusion"));
		combine = CreatePass("apply.frag", "result", blur_samplers);

		unordered_map<string, string> antialiasing_samplers;
		antialiasing_samplers.insert(make_pair("image_tex",    "result"));
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
			glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Projection));
			glUseProgram(0);
		}

		if(GLuint id = Entity->Get<StorageShader>(antialiasing)->Program)
		{
			Vector2u Size = Global->Get<RenderWindow>("window")->getSize();
			glUseProgram(id);
			glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));
			glUseProgram(0);
		}

		if(GLuint id = Entity->Get<StorageShader>(combine)->Program)
		{
			Vector2u Size = Global->Get<RenderWindow>("window")->getSize();
			glUseProgram(id);
			glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));
			glUseProgram(0);
		}
	}

	unsigned int CreatePass(string PathFragment, string Target, pair<string, string> Sampler, float Size = 1.0)
	{
		unordered_map<string, string> samplers;
		samplers.insert(Sampler);
		return CreatePass(PathFragment, Target, samplers, Size);
	}

	unsigned int CreatePass(string PathFragment, string Target, unordered_map<string, string> Samplers = unordered_map<string, string>(), float Size = 1.0)
	{
		unordered_map<GLenum, pair<string, GLenum>> targets;
		targets.insert(make_pair(GL_COLOR_ATTACHMENT0, make_pair(Target, GL_RGBA32F)));
		return CreatePass("quad.vert", PathFragment, targets, Samplers, Size);
	}

	unsigned int CreatePass(string PathVertex, string PathFragment, unordered_map<GLenum, pair<string, GLenum>> Targets, unordered_map<string, string> Samplers = unordered_map<string, string>(), float Size = 1.0)
	{
		unsigned int id = Entity->New();
		auto shd = Entity->Add<StorageShader>(id);
		auto frb = Entity->Add<StorageFramebuffer>(id);

		shd->PathVertex   = PathVertex;
		shd->PathFragment = PathFragment;
		for(auto i : Samplers)
			shd->Samplers.insert(make_pair(i.first, Textures[i.second]));
		
		frb->Size = Size;
		for(auto i : Targets)
		{
			GLuint id = frb->AddTarget(i.first, i.second.second);
			Textures.insert(make_pair(i.second.first, id));
		}

		return id;
	}
};
