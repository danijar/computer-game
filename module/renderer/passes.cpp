#include "module.h"

#include <dependency/sfml/OpenGL.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
#include <dependency/glm/gtc/type_ptr.hpp>
using namespace std;
using namespace sf;
using namespace glm;

#include "type/camera/type.h"

void ModuleRenderer::PassCreate(string Name, string Vertex, string Fragment, unordered_map<GLenum, string> Targets, unordered_map<string, string> Samplers, unordered_map<string, string> Copyfallbacks, unordered_map<string, vec3> Colorfallbacks, Function Function, bool Clear, float Size, GLenum StencilFunction, GLint StencilReference, GLenum StencilOperation)
{
	if(PassGet(Name, false))
	{
		Log->Fail("pass (" + Name + ") already exists");
		return;
	}

	Pass pass = PassCreate(Vertex, Fragment, Targets, Samplers, Copyfallbacks, Colorfallbacks, Function, Clear, Size, StencilFunction, StencilReference, StencilOperation);
	passes.push_back(make_pair(Name, pass));
}

ModuleRenderer::Pass ModuleRenderer::PassCreate(string Vertex, string Fragment, unordered_map<GLenum, string> Targets, unordered_map<string, string> Samplers, unordered_map<string, string> Copyfallbacks, unordered_map<string, vec3> Colorfallbacks, Function Function, bool Clear, float Size, GLenum StencilFunction, GLint StencilReference, GLenum StencilOperation)
{
	Pass pass;

	for(auto i : Targets)
	{
		auto texture = TextureGet(i.second);
		if(get<2>(texture) != Size)
		{
			Log->Fail("size of target (" + i.second + ") and pass must match");
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

	for(auto i : Copyfallbacks)
	{
		GLuint to   = get<0>(TextureGet(i.first)),
			   from = get<0>(TextureGet(i.second));
		if(to && from) pass.Copyfallbacks.insert(make_pair(to, from));
	}

	for(auto i : Colorfallbacks)
	{
		GLuint to = get<0>(TextureGet(i.first));
		if(to) pass.Colorfallbacks.insert(make_pair(to, i.second));
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
		Log->Fail("pass invalid drawing function");
	}

	pass.Vertex           = Vertex;
	pass.Fragment         = Fragment;
	pass.Clear            = Clear;
	pass.Size             = Size;
	pass.Program          = CreateProgram(Vertex, Fragment);
	pass.Framebuffer      = Function == SCREEN ? 0 : FramebufferCreate(pass.Targets);
	pass.StencilFunction  = StencilFunction;
	pass.StencilReference = StencilReference;
	pass.StencilOperation = StencilOperation;

	return pass;
}

ModuleRenderer::Pass *ModuleRenderer::PassGet(string Name, bool Output)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	if(Output) Log->Fail("cannot get pass because (" + Name + ") doesn't exist");
	return NULL;
}
