#include "module.h"

#include <SFML/Window.hpp>
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
	DrawForms(GetPass("form"));

	DrawLight(GetPass("light"));

	for(unsigned int i = 2; i < passes.size() - 1; ++i)
		DrawQuad(&passes[i].second);

	DrawQuad(&passes.back().second, true);
}

void ModuleRenderer::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			// check whether file actually changed
			glDeleteProgram(i->second.Shader);
			i->second.Shader = CreateProgram(i->second.Vertex, i->second.Fragment);
		}

		Uniforms();
	});

	Event->Listen("WindowRecreated", [=]{
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			glDeleteFramebuffers(1, &i->second.Framebuffer);
			i->second.Framebuffer = CreateFramebuffer(i->second.Targets, i->second.Samplers, i->second.Size);
		}

		Uniforms();
	});

	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		for(auto i : passes)
			for(auto j : i.second.Targets)
				TextureResize(j.second.first, j.second.second, Vector2u(Vector2f(Size) * i.second.Size));

		Uniforms();
	});

	Event->Listen("ShaderUpdated", [=]{
		Uniforms();
	});
}
