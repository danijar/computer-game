#include "module.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace std;
using namespace sf;
using namespace glm;

#include "camera.h"


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
	if(PassGet(Name, false))
	{
		Debug->Fail("pass (" + Name + ") already exists");
		return;
	}

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
		pass.Function = bind(&ModuleRenderer::DrawForms,  this, std::placeholders::_1);
		break;
	case SKY:
		pass.Function = bind(&ModuleRenderer::DrawSky,    this, std::placeholders::_1);
		break;
	case LIGHTS:
		pass.Function = bind(&ModuleRenderer::DrawLights, this, std::placeholders::_1);
		break;
	case QUAD:
		pass.Function = bind(&ModuleRenderer::DrawQuad,   this, std::placeholders::_1);
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

ModuleRenderer::Pass *ModuleRenderer::PassGet(string Name, bool Output)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	if(Output) Debug->Fail("cannot get pass because (" + Name + ") doesn't exist");
	return NULL;
}
