#include "module.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace std;
using namespace sf;
using namespace glm;

#include "camera.h"


void ModuleRenderer::Pipeline()
{
	unordered_map<GLenum, pair<string, GLenum>> forms_targets;
	forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT0, make_pair("position", GL_RGBA32F)));
	forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT1, make_pair("normal",   GL_RGBA32F)));
	forms_targets.insert(make_pair(GL_COLOR_ATTACHMENT2, make_pair("albedo",   GL_RGBA32F)));
	forms_targets.insert(make_pair(GL_DEPTH_ATTACHMENT,  make_pair("depth",    GL_DEPTH_COMPONENT32F)));
	CreatePass("form", "forms.vert", "forms.frag", forms_targets);
	
	unordered_map<string, string> light_samplers;
	light_samplers.insert(make_pair("positions", "position"));
	light_samplers.insert(make_pair("normals",   "normal"));
	CreatePass("light", "light.frag", "light", light_samplers);

	unordered_map<string, string> edge_samplers;
	edge_samplers.insert(make_pair("position_tex", "position"));
	edge_samplers.insert(make_pair("normal_tex",   "normal"));
	CreatePass("edge", "edge.frag", "edge", edge_samplers);

	unordered_map<string, string> combine_samplers;
	combine_samplers.insert(make_pair("albedo", "albedo"));
	combine_samplers.insert(make_pair("lights", "light"));
	CreatePass("combine", "combine.frag", "image", combine_samplers);

	unordered_map<string, string> occlusion_samplers;
	occlusion_samplers.insert(make_pair("position_tex", "position"));
	occlusion_samplers.insert(make_pair("normal_tex",   "normal"));
	CreatePass("occlusion", "occlusion.frag", "occlusion", occlusion_samplers, 0.5);
	GetPass("occlusion")->Samplers.insert(make_pair("noise_tex", CreateTexture("noise.png", true, false, false)));

	unordered_map<string, string> apply_samplers;
	apply_samplers.insert(make_pair("image_tex",  "image"));
	apply_samplers.insert(make_pair("effect_tex", "occlusion"));
	apply_samplers.insert(make_pair("edge_tex",   "edge"));
	CreatePass("apply", "apply.frag", "result", apply_samplers);

	CreatePass("blur_u", "blur_u.frag", "temp", make_pair("image_tex", "result"));
	CreatePass("blur_v", "blur_v.frag", "blur", make_pair("image_tex", "temp"));

	unordered_map<string, string> antialiasing_samplers;
	antialiasing_samplers.insert(make_pair("image_tex", "result"));
	antialiasing_samplers.insert(make_pair("blur_tex",  "blur"));
	antialiasing_samplers.insert(make_pair("edge_tex",  "edge"));
	CreatePass("antialiasing", "antialiasing.frag", "antialiasing", antialiasing_samplers);

	CreatePass("screen", "screen.frag", "screen", make_pair("image_tex", "antialiasing"));
}

void ModuleRenderer::Uniforms()
{
	GLuint id;
	
	id = GetPass("form")->Shader;
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Projection));

	Vector2u Size = Global->Get<RenderWindow>("window")->getSize();

	id = GetPass("edge")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("combine")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("combine")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("apply")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));
	
	id = GetPass("blur_u")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));
	id = GetPass("blur_v")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frameBufSize"), 1, value_ptr(vec2(Size.x, Size.y)));

	glUseProgram(0);
}

ModuleRenderer::Pass ModuleRenderer::CreatePass(string Name, string Fragment, string Target, pair<string, string> Sampler, float Size)
{
	unordered_map<string, string> samplers;
	samplers.insert(Sampler);
	return CreatePass(Name, Fragment, Target, samplers, Size);
}

ModuleRenderer::Pass ModuleRenderer::CreatePass(string Name, string Fragment, string Target, unordered_map<string, string> Samplers, float Size)
{
	unordered_map<GLenum, pair<string, GLenum>> targets;
	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, make_pair(Target, GL_RGBA32F)));
	return CreatePass(Name, "quad.vert", Fragment, targets, Samplers, Size);
}

ModuleRenderer::Pass ModuleRenderer::CreatePass(string Name, string Vertex, string Fragment, unordered_map<GLenum, pair<string, GLenum>> Targets, unordered_map<string, string> Samplers, float Size)
{
	Pass pass;

	for(auto i : Samplers)
	{
		auto sampler = targets.find(i.second);
		if(sampler == targets.end())
			Debug->Fail("sampler (" + i.second + ") not found");
		else
			pass.Samplers.insert(make_pair(i.first, sampler->second));
	}

	for(auto i : Targets)
	{
		GLuint id;
		auto target = targets.find(i.second.first);
		if(target == targets.end())
		{
			glGenTextures(1, &id);
			targets.insert(make_pair(i.second.first, id));
		}
		else
			id = target->second;
		pass.Targets.insert(make_pair(i.first, make_pair(id, i.second.second)));
	}

	pass.Vertex      = Vertex;
	pass.Fragment    = Fragment;
	pass.Size        = Size;
	pass.Shader      = CreateProgram(Vertex, Fragment);
	pass.Framebuffer = CreateFramebuffer(pass.Targets, pass.Samplers, pass.Size);

	passes.push_back(make_pair(Name, pass));
	return pass;
}

ModuleRenderer::Pass *ModuleRenderer::GetPass(string Name)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	Debug->Fail("cannot get pass because " + Name + " doesn't exist.");
	return new Pass();
}
