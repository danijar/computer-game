#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;


void ModuleRenderer::Init()
{
	Opengl->Init();

	Pipeline();
	Uniforms();

	Listeners();
	Callbacks();
}

void ModuleRenderer::Update()
{
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	//glEnable(GL_STENCIL_TEST);

	for(auto i : passes)
	{
		Pass pass = i.second;

		if(pass.Enabled)
		{
			glUseProgram(pass.Program);
			//glStencilFunc(pass.StencilFunction, pass.StencilReference, 1);
			//glStencilOp(GL_KEEP, pass.StencilOperation, pass.StencilOperation);
			glViewport(0, 0, int(size.x * pass.Size), int(size.y * pass.Size));

			pass.Function(&pass); // later on bind pass pointer, too
		}
		else
		{
			// apply fallbacks
		}
	}

	glDisable(GL_STENCIL_TEST);
	glUseProgram(0);

	Opengl->Test();
}

void ModuleRenderer::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			// check whether file actually changed
			glDeleteProgram(i->second.Program);
			i->second.Program = CreateProgram(i->second.Vertex, i->second.Fragment);
		}
		Uniforms();
	});

	Event->Listen("WindowRecreated", [=]{
		for(auto i : textures)
			TextureResize(get<0>(i.second), get<1>(i.second), get<2>(i.second));
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			glDeleteFramebuffers(1, &i->second.Framebuffer);
			i->second.Framebuffer = CreateFramebuffer(i->second.Targets);
		}
		Uniforms();
	});

	Event->Listen("WindowResize", [=]{
		for(auto i : textures)
			TextureResize(get<0>(i.second), get<1>(i.second), get<2>(i.second));
		Uniforms();
	});

	Event->Listen("ShaderUpdated", [=]{
		Uniforms();
	});
}
