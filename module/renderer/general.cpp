#include "module.h"

#include <dependency/sfml/Window.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;

#include "type/print/type.h"


void ModuleRenderer::Init()
{
	Opengl->Init();
	
	Callbacks();

	Script->Run("pipeline.js");
	Script->Run("uniforms.js");

	Listeners();

	color = PassCreate("quad.vert", "color.frag");
	copy = PassCreate("quad.vert", "copy.frag");

	query = 0, time = 0;
	Entity->Add<Print>(Entity->New())->Text = [=]{
		float average = Logic->Average("render time", time / 1000000.0f, 60);
		return "rendering time " + Log->Format(average, 2, 1) + " ms";
	};
}

void ModuleRenderer::Update()
{
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	// measure rendering time
	if (query) {
		GLint done = 0;
		while (!done) glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time);
	}
	glGenQueries(1, &query);
	glBeginQuery(GL_TIME_ELAPSED, query);

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
			if(pass.Clear) {
				glClearColor(pass.Clearcolor.r, pass.Clearcolor.g, pass.Clearcolor.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
			}
			if(pass.Depth) {
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
			}
			glUseProgram(pass.Program);
			pass.Function(&pass);
			if(pass.Depth) {
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
			}
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
				if(pass.Clear) glClear(GL_COLOR_BUFFER_BIT);

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
				if(pass.Clear) glClear(GL_COLOR_BUFFER_BIT);

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

	glEndQuery(GL_TIME_ELAPSED);

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
