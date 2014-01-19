#include "module.h"

#include <dependency/sfml/Window.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;


void ModuleRenderer::Init()
{
	Opengl->Init();
	
	Callbacks();

	Script->Run("pipeline.js");
	Script->Run("uniforms.js");

	Listeners();

	color = PassCreate("quad.vert", "color.frag");
	copy = PassCreate("quad.vert", "copy.frag");
}

void ModuleRenderer::Update()
{
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	glEnable(GL_STENCIL_TEST);

	for(auto i : passes)
	{
		Pass pass = i.second;

		// set render pass options
		glStencilFunc(pass.StencilFunction, pass.StencilReference, 0xFF);
		glStencilOp(GL_KEEP, pass.StencilOperation, pass.StencilOperation);
		glViewport(0, 0, int(size.x * pass.Size), int(size.y * pass.Size));

		if(pass.Enabled)
		{
			// call render function
			glBindFramebuffer(GL_FRAMEBUFFER, pass.Framebuffer);
			if(pass.Clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glUseProgram(pass.Program);
			pass.Function(&pass);
		}
		else
		{
			// apply copy fallbacks
			glBindFramebuffer(GL_FRAMEBUFFER, copy.Framebuffer);
			glUseProgram(copy.Program);
			for(auto j : pass.Copyfallbacks)
			{
				// attach fallback target and stencil
				unordered_map<GLenum, GLuint> targets;
				targets.insert(make_pair(GL_COLOR_ATTACHMENT0, j.first));
				auto depthstencil = pass.Targets.find(GL_DEPTH_STENCIL_ATTACHMENT);
				if(depthstencil != pass.Targets.end())
					targets.insert(make_pair(depthstencil->first, depthstencil->second));
				FramebufferTargets(copy.Framebuffer, targets, false);

				// clear
				if(pass.Clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				// copy texture over
				copy.Samplers.clear();
				copy.Samplers.insert(make_pair("copy", j.second));
				copy.Function(&copy);
			}

			// apply color fallbacks
			glBindFramebuffer(GL_FRAMEBUFFER, color.Framebuffer);
			glUseProgram(color.Program);
			for(auto j : pass.Colorfallbacks)
			{
				// attach fallback target and stencil
				unordered_map<GLenum, GLuint> targets;
				targets.insert(make_pair(GL_COLOR_ATTACHMENT0, j.first));
				auto depthstencil = pass.Targets.find(GL_DEPTH_STENCIL_ATTACHMENT);
				if(depthstencil != pass.Targets.end())
					targets.insert(make_pair(depthstencil->first, depthstencil->second));
				FramebufferTargets(copy.Framebuffer, targets, false);

				// clear
				if(pass.Clear) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				// fill with color
				float array[3] = { j.second.r, j.second.g, j.second.b };
				glUniform3fv(glGetUniformLocation(color.Program, "color"), 1, array);
				color.Function(&color);
			}
		}
	}

	glDisable(GL_STENCIL_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		this->Event->Fire("ShaderUpdated");
	});

	Event->Listen("WindowRecreated", [=]{
		for(auto i : textures)
			if(get<2>(i.second))
				TextureResize(get<0>(i.second), get<1>(i.second), get<2>(i.second));
		for(auto i = passes.begin(); i != passes.end(); ++i)
		{
			glDeleteFramebuffers(1, &i->second.Framebuffer);
			i->second.Framebuffer = FramebufferCreate(i->second.Targets);
		}
	});

	Event->Listen("WindowResize", [=]{
		for(auto i : textures)
			if(get<2>(i.second))
				TextureResize(get<0>(i.second), get<1>(i.second), get<2>(i.second));
	});
}
