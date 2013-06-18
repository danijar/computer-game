#include "module.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace std;
using namespace sf;
using namespace glm;

#include "camera.h"


void ModuleRenderer::Pipeline()
{
	unordered_map<GLenum, string> targets;
	unordered_map<string, string> samplers;
	unordered_map<string, string> fallbacks;

	TextureCreate("position", GL_RGB16F);
	TextureCreate("normal", GL_RGB16F);
	TextureCreate("albedo", GL_RGB16F);
	TextureCreate("depth", GL_DEPTH24_STENCIL8);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "position"));
	targets.insert(make_pair(GL_COLOR_ATTACHMENT1, "normal"));
	targets.insert(make_pair(GL_COLOR_ATTACHMENT2, "albedo"));
	targets.insert(make_pair(GL_DEPTH_STENCIL_ATTACHMENT, "depth"));
	CreatePass("form", "forms.vert", "forms.frag", targets, samplers, fallbacks, FORMS, 1.0f, GL_ALWAYS, 1, GL_REPLACE);
	targets.clear();

	TextureCreate("light", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "light"));
	targets.insert(make_pair(GL_DEPTH_STENCIL_ATTACHMENT, "depth"));
	samplers.insert(make_pair("positions", "position"));
	samplers.insert(make_pair("normals",   "normal"));
	CreatePass("light", "quad.vert", "light.frag", targets, samplers, fallbacks, LIGHTS);
	targets.clear();
	samplers.clear();

	TextureCreate("edge", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "edge"));
	samplers.insert(make_pair("depth_tex", "depth"));
	samplers.insert(make_pair("normal_tex", "normal"));
	CreatePass("edge", "quad.vert", "edge.frag", targets, samplers, fallbacks, QUAD, 1.0f, GL_GEQUAL, 1);
	targets.clear();
	samplers.clear();

	TextureCreate("image", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "image"));
	samplers.insert(make_pair("albedo", "albedo"));
	samplers.insert(make_pair("lights", "light"));
	samplers.insert(make_pair("depth", "depth"));
	CreatePass("combine", "quad.vert", "combine.frag", targets, samplers, fallbacks, QUAD, 1.0f, GL_GEQUAL, 1);
	targets.clear();
	samplers.clear();

	TextureCreate("occlusion", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "occlusion"));
	samplers.insert(make_pair("depth_tex", "albedo"));
	samplers.insert(make_pair("lights", "depth"));
	samplers.insert(make_pair("normal_tex", "normal"));
	CreatePass("occlusion", "quad.vert", "occlusion.frag", targets, samplers, fallbacks, QUAD, 0.75f, GL_GEQUAL, 1);
	targets.clear();
	samplers.clear();
	GetPass("occlusion")->Samplers.insert(make_pair("noise_tex", CreateTexture("noise.png", true, false, false)));

	TextureCreate("result", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "result"));
	samplers.insert(make_pair("image_tex", "image"));
	samplers.insert(make_pair("effect_tex", "occlusion"));
	CreatePass("apply", "quad.vert", "apply.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("temp", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "temp"));
	samplers.insert(make_pair("image_tex", "result"));
	CreatePass("blur_u", "quad.vert", "blur_u.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("blur", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "blur"));
	samplers.insert(make_pair("image_tex", "temp"));
	CreatePass("blur_v", "quad.vert", "blur_v.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("antialiasing", GL_RGB16F);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "antialiasing"));
	samplers.insert(make_pair("image_tex", "result"));
	samplers.insert(make_pair("blur_tex", "blur"));
	samplers.insert(make_pair("edge_tex", "edge"));
	CreatePass("screen", "quad.vert", "antialiasing.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("screen", GL_RGB16F);
	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "screen"));
	samplers.insert(make_pair("image_tex", "antialiasing"));
	CreatePass("screen", "quad.vert", "screen.frag", targets, samplers, fallbacks, SCREEN);
}

void ModuleRenderer::Uniforms()
{
	GLuint id;
	
	id = GetPass("form")->Program;
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Projection));

	Vector2u Size = Global->Get<RenderWindow>("window")->getSize();

	id = GetPass("edge")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("combine")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("occlusion")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	
	id = GetPass("blur_u")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	id = GetPass("blur_v")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	glUseProgram(0);
}

void ModuleRenderer::CreatePass(string Name, string Vertex, string Fragment, unordered_map<GLenum, string> Targets, unordered_map<string, string> Samplers, unordered_map<string, string> Fallbacks, Function Function, float Size, GLenum StencilFunction, GLint StencilReference, GLenum StencilOperation)
{
	Pass pass;

	for(auto i : Targets)
	{
		GLuint id = TextureGet(i.second);
		if(id) pass.Targets.insert(make_pair(i.first, id));
	}

	for(auto i : Samplers)
	{
		GLuint id = TextureGet(i.second);
		if(id)  pass.Samplers.insert(make_pair(i.first, id));
	}

	for(auto i : Fallbacks)
	{
		GLuint from = TextureGet(i.second),
			   to   = TextureGet(i.first);
		if(from && to) pass.Fallbacks.insert(make_pair(from, to));
	}

	switch(Function)
	{
	case FORMS:
		pass.Function = bind(&ModuleRenderer::DrawForms, this, std::placeholders::_1);
		break;
	case LIGHTS:
		pass.Function = bind(&ModuleRenderer::DrawLights, this, std::placeholders::_1);
		break;
	case QUAD:
		pass.Function = bind(&ModuleRenderer::DrawQuad, this, std::placeholders::_1);
		break;
	case SCREEN:
		pass.Function = bind(&ModuleRenderer::DrawScreen, this, std::placeholders::_1);
		break;
	}

	pass.Vertex           = Vertex;
	pass.Fragment         = Fragment;
	pass.Size             = Size;
	pass.Program          = CreateProgram(Vertex, Fragment);
	pass.Framebuffer      = CreateFramebuffer(pass.Targets);
	pass.StencilFunction  = StencilFunction;
	pass.StencilReference = StencilReference;
	pass.StencilOperation = StencilOperation;

	passes.push_back(make_pair(Name, pass));
}

ModuleRenderer::Pass *ModuleRenderer::GetPass(string Name)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	Debug->Fail("cannot get pass because " + Name + " doesn't exist.");
	return new Pass();
}
