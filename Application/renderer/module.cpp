#pragma once

#include "module.h"

using namespace std;
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "settings.h"
#include "framebuffer.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "light.h"


void ModuleRenderer::Init()
{
	Opengl->Init();
	Pipeline();
	Uniforms();
	Listeners();
}

void ModuleRenderer::Update()
{
	auto fbs = Entity->Get<StorageFramebuffer>();
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uint n = 0;
	for(auto i : fbs)
	{
		auto fbr = i.second;
		auto shd = Entity->Get<StorageShader>(i.first);
		if(fbr->Id && shd->Program)
		{
			glViewport(0, 0, (int)(size.x * fbr->Size), (int)(size.y * fbr->Size));

			if(n < fbs.size() - 1)
				glBindFramebuffer(GL_FRAMEBUFFER, fbr->Id);
			else
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

			if(n == 0)
				Forms(shd->Program, shd->Samplers);
			else if(n == 1)
				Light(shd->Program, shd->Samplers);
			else
				Quad (shd->Program, shd->Samplers);
		}
		n++;
	}

	auto shs = Entity->Get<StorageShader>();
	int Count = 0;
	for(auto i = shs.begin(); i != shs.end(); ++i)
	{
		if(i->second->Changed)
		{
			i->second->Program = CreateProgram(i->second->PathVertex, i->second->PathFragment);
			i->second->Changed = false;
			Count++;
		}
	}
	if(Count > 0)
	{
		Debug->Print("reloaded " + to_string(Count));
		this->Event->Fire("ShaderUpdated");
	}

	// auto fbs = Entity->Get<StorageFramebuffer>();
	for(auto i = fbs.begin(); i != fbs.end(); ++i)
	{
		if(!i->second->Id)
		{
			glGenFramebuffers(1, &i->second->Id);
			Setup(i->first);
		}
	}
}

void ModuleRenderer::Listeners()
{
	Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
		auto stg = Global->Get<StorageSettings>("settings");
		switch(Code)
		{
		case Keyboard::Key::F3:
			stg->Wireframe = !stg->Wireframe;
			break;
		}
	});

	Event->Listen("WindowFocusGained", [=]{
		auto shs = Entity->Get<StorageShader>();
		for(auto i = shs.begin(); i != shs.end(); ++i)
		{
			// check if the file actually changed
			i->second->Changed = true;
		}
	});

	Event->Listen("WindowResize", [=]{
		Uniforms();
	});

	Event->Listen("ShaderUpdated", [=]{
		Uniforms();
	});

	Event->Listen("WindowRecreated", [=]{
		auto fbs = Entity->Get<StorageFramebuffer>();
		for(auto i = fbs.begin(); i != fbs.end(); ++i)
		{
			glDeleteFramebuffers(1, &i->second->Id);
			glGenFramebuffers(1, &i->second->Id);
			Setup(i->first);
		}

		Uniforms();
	});

	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		Resize(Size);
	});
}
