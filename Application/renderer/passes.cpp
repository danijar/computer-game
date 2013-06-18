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
	CreatePass("form", "forms.vert", "forms.frag", Targets(
		GL_COLOR_ATTACHMENT0,        "position", GL_RGB16F,
		GL_COLOR_ATTACHMENT1,        "normal",   GL_RGB16F,
		GL_COLOR_ATTACHMENT2,        "albedo",   GL_RGB16F,
		GL_DEPTH_STENCIL_ATTACHMENT, "depth",    GL_DEPTH24_STENCIL8
		), Samplers(), bind(&ModuleRenderer::DrawForms, this, std::placeholders::_1) /*[=](Pass *p){ DrawForms(p); }*/);

	CreatePass("light", "light.frag", Targets(
		GL_COLOR_ATTACHMENT0,        "light", GL_RGB16F,
		GL_DEPTH_STENCIL_ATTACHMENT, "depth", GL_DEPTH24_STENCIL8
	), Samplers(
		"positions", "position",
		"normals",   "normal"
	), [=](Pass *p){ DrawLight(p); });

	CreatePass("edge", "edge.frag", Targets(
		GL_COLOR_ATTACHMENT0, "edge"
	), Samplers(
		"depth_tex",  "depth",
		"normal_tex", "normal"
	), [=](Pass *p){ DrawQuadStenciled(p); });

	CreatePass("combine", "combine.frag", Targets(
		GL_COLOR_ATTACHMENT0, "image"//, GL_RGB16,
		//GL_DEPTH_STENCIL_ATTACHMENT, "depth", GL_DEPTH24_STENCIL8
	), Samplers(
		"albedo", "albedo",
		"lights", "light",
		"depth",  "depth"
	), [=](Pass *p){ DrawQuadStenciled(p); });

	CreatePass("occlusion", "occlusion.frag", Targets(
		GL_COLOR_ATTACHMENT0, "occlusion"
	), Samplers(
		"depth_tex",  "depth",
		"normal_tex", "normal"
	), [=](Pass *p){ DrawQuadStenciled(p); }, 0.75);
	GetPass("occlusion")->Samplers.insert(make_pair("noise_tex", CreateTexture("noise.png", true, false, false)));

	CreatePass("apply", "apply.frag", Targets(
		GL_COLOR_ATTACHMENT0, "result"
	), Samplers(
		"image_tex",  "image",
		"effect_tex", "occlusion"
	), [=](Pass *p){ DrawQuadStenciled(p); });

	CreatePass("blur_u", "blur_u.frag", Targets(GL_COLOR_ATTACHMENT0, "temp", GL_RGB16), Samplers("image_tex", "result"), [=](Pass *p){ DrawQuadStenciled(p); });
	CreatePass("blur_v", "blur_v.frag", Targets(GL_COLOR_ATTACHMENT0, "blur", GL_RGB16), Samplers("image_tex", "temp"), [=](Pass *p){ DrawQuadStenciled(p); });

	CreatePass("antialiasing", "antialiasing.frag", Targets(
		GL_COLOR_ATTACHMENT0, "antialiasing"
	), Samplers(
		"image_tex", "result",
		"blur_tex",  "blur",
		"edge_tex",  "edge"
	), [=](Pass *p){ DrawQuadStenciled(p); });

	CreatePass("screen", "screen.frag", Targets(GL_COLOR_ATTACHMENT0, "screen"), Samplers("image_tex", "antialiasing"), [=](Pass *p){ DrawQuadScreen(p); });
}

void ModuleRenderer::Uniforms()
{
	GLuint id;
	
	id = GetPass("form")->Shader;
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, value_ptr(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Projection));

	Vector2u Size = Global->Get<RenderWindow>("window")->getSize();

	id = GetPass("edge")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("combine")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	id = GetPass("occlusion")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	
	id = GetPass("blur_u")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));
	id = GetPass("blur_v")->Shader;
	glUseProgram(id);
	glUniform2fv(glGetUniformLocation(id, "frame_size"), 1, value_ptr(vec2(Size.x, Size.y)));

	glUseProgram(0);
}

ModuleRenderer::Pass ModuleRenderer::CreatePass(string Name, string Fragment, TargetList Targets, SamplerList Samplers, function<void(Pass*)> Drawfunction, float Size, GLenum StencilFunction, GLint StencilReference)
{
	return CreatePass(Name, "quad.vert", Fragment, Targets, Samplers, Drawfunction, Size, StencilFunction, StencilReference);
}

ModuleRenderer::Pass ModuleRenderer::CreatePass(string Name, string Vertex, string Fragment, TargetList Targets, SamplerList Samplers, function<void(Pass*)> Drawfunction, float Size, GLenum StencilFunction, GLint StencilReference)
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

	pass.Vertex           = Vertex;
	pass.Fragment         = Fragment;
	pass.Size             = Size;
	pass.Shader           = CreateProgram(Vertex, Fragment);
	pass.Framebuffer      = CreateFramebuffer(pass.Targets, pass.Size);
	pass.Drawfunction     = Drawfunction;
	pass.StencilFunction  = StencilFunction;
	pass.StencilReference = StencilReference;

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

ModuleRenderer::TargetList ModuleRenderer::Targets(GLenum attachment1, string texture1, GLenum type1, GLenum attachment2, string texture2, GLenum type2, GLenum attachment3, string texture3, GLenum type3, GLenum attachment4, string texture4, GLenum type4)
{
	TargetList list;
	if(attachment1 && texture1 != "") list.insert(std::make_pair(attachment1, std::make_pair(texture1, type1)));
	if(attachment2 && texture2 != "") list.insert(std::make_pair(attachment2, std::make_pair(texture2, type2)));
	if(attachment3 && texture3 != "") list.insert(std::make_pair(attachment3, std::make_pair(texture3, type3)));
	if(attachment4 && texture4 != "") list.insert(std::make_pair(attachment4, std::make_pair(texture4, type4)));
	return list;
}

ModuleRenderer::SamplerList ModuleRenderer::Samplers(string sampler1, string texture1, string sampler2, string texture2, string sampler3, string texture3, string sampler4, string texture4)
{
	SamplerList list;
	if(sampler1 != "" && texture1 != "") list.insert(std::make_pair(sampler1, texture1));
	if(sampler2 != "" && texture2 != "") list.insert(std::make_pair(sampler2, texture2));
	if(sampler3 != "" && texture3 != "") list.insert(std::make_pair(sampler3, texture3));
	if(sampler4 != "" && texture4 != "") list.insert(std::make_pair(sampler4, texture4));
	return list;
}
