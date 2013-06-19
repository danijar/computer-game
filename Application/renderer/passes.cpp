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

	TextureCreate("position");
	TextureCreate("normal");
	TextureCreate("albedo");
	TextureCreate("depth", GL_DEPTH24_STENCIL8);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "position"));
	targets.insert(make_pair(GL_COLOR_ATTACHMENT1, "normal"));
	targets.insert(make_pair(GL_COLOR_ATTACHMENT2, "albedo"));
	targets.insert(make_pair(GL_DEPTH_STENCIL_ATTACHMENT, "depth"));
	PassCreate("forms", "forms.vert", "forms.frag", targets, samplers, fallbacks, FORMS, 1.0f, GL_ALWAYS, 1, GL_REPLACE);
	PassCreate("sky",   "forms.vert", "forms.frag", targets, samplers, fallbacks, SKY,   1.0f, GL_EQUAL, 0, GL_KEEP);
	targets.clear();

	TextureCreate("light");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "light"));
	targets.insert(make_pair(GL_DEPTH_STENCIL_ATTACHMENT, "depth"));
	samplers.insert(make_pair("positions", "position"));
	samplers.insert(make_pair("normals", "normal"));
	PassCreate("lights", "quad.vert", "light.frag", targets, samplers, fallbacks, LIGHTS, 1.0f, GL_GEQUAL, 1);
	targets.clear();
	samplers.clear();

	TextureCreate("edge");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "edge"));
	samplers.insert(make_pair("depth_tex", "depth"));
	samplers.insert(make_pair("normal_tex", "normal"));
	PassCreate("edge", "quad.vert", "edge.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("image");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "image"));
	samplers.insert(make_pair("albedo", "albedo"));
	samplers.insert(make_pair("lights", "light"));
	samplers.insert(make_pair("depth", "depth"));
	PassCreate("combine", "quad.vert", "combine.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("occlusion", GL_RGB16F, 0.75f);

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "occlusion"));
	samplers.insert(make_pair("depth_tex", "depth"));
	samplers.insert(make_pair("normal_tex", "normal"));
	PassCreate("occlusion", "quad.vert", "occlusion.frag", targets, samplers, fallbacks, QUAD, 0.75f, GL_GEQUAL, 1);
	targets.clear();
	samplers.clear();
	PassGet("occlusion")->Samplers.insert(make_pair("noise_tex", CreateTexture("noise.png", true, false, false)));

	TextureCreate("result");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "result"));
	samplers.insert(make_pair("image_tex", "image"));
	samplers.insert(make_pair("effect_tex", "occlusion"));
	PassCreate("apply", "quad.vert", "apply.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("temp");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "temp"));
	samplers.insert(make_pair("image_tex", "result"));
	PassCreate("blur_u", "quad.vert", "blur_u.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("blur");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "blur"));
	samplers.insert(make_pair("image_tex", "temp"));
	PassCreate("blur_v", "quad.vert", "blur_v.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("antialiasing");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "antialiasing"));
	samplers.insert(make_pair("image_tex", "result"));
	samplers.insert(make_pair("blur_tex", "blur"));
	samplers.insert(make_pair("edge_tex", "edge"));
	PassCreate("screen", "quad.vert", "antialiasing.frag", targets, samplers, fallbacks, QUAD);
	targets.clear();
	samplers.clear();

	TextureCreate("screen");

	targets.insert(make_pair(GL_COLOR_ATTACHMENT0, "screen"));
	samplers.insert(make_pair("image_tex", "antialiasing"));
	PassCreate("screen", "quad.vert", "screen.frag", targets, samplers, fallbacks, SCREEN);
}

void ModuleRenderer::Uniforms()
{
	GLuint id;
	
	id = PassGet("forms")->Program;
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Projection));

	id = PassGet("sky")->Program;
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Projection));

	Vector2u Size = Global->Get<RenderWindow>("window")->getSize();

	id = PassGet("edge")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = PassGet("combine")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = PassGet("occlusion")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	
	id = PassGet("blur_u")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	id = PassGet("blur_v")->Program;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	glUseProgram(0);
}

void ModuleRenderer::PassCreate(string Name, string Vertex, string Fragment, unordered_map<GLenum, string> Targets, unordered_map<string, string> Samplers, unordered_map<string, string> Fallbacks, Function Function, float Size, GLenum StencilFunction, GLint StencilReference, GLenum StencilOperation)
{
	//if(PassGet(Name) 

	Pass pass;

	for(auto i : Targets)
	{
		auto texture = TextureGet(i.second);
		if(get<2>(texture) != Size)
		{
			Debug->Fail("size of pass (" + Name + ") and target (" + i.second + ") must match");
			continue;
		}
		GLuint id = get<0>(texture);
		if(id) pass.Targets.insert(make_pair(i.first, id));
	}

	for(auto i : Samplers)
	{
		GLuint id = get<0>(TextureGet(i.second));
		if(id) pass.Samplers.insert(make_pair(i.first, id));
	}

	for(auto i : Fallbacks)
	{
		GLuint from = get<0>(TextureGet(i.second)),
			   to   = get<0>(TextureGet(i.first));
		if(from && to) pass.Fallbacks.insert(make_pair(from, to));
	}

	switch(Function)
	{
	case FORMS:
		pass.Function = bind(&ModuleRenderer::DrawForms, this, std::placeholders::_1);
		break;
	case SKY:
		pass.Function = bind(&ModuleRenderer::DrawSky, this, std::placeholders::_1);
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
	default:
		Debug->Fail("pass (" + Name + ") invalid drawing function");
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

ModuleRenderer::Pass *ModuleRenderer::PassGet(string Name)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	Debug->Fail("cannot get pass because " + Name + " doesn't exist.");
	return new Pass();
}
