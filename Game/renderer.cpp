#pragma once

#include "system.h"

#include "window.h"

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
using namespace sf;
// maybe include GLM


class ComponentRenderer : public Component
{

	void Init()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		wnd->setVerticalSyncEnabled(true);

		glewExperimental = GL_TRUE;
		glewInit();

		Listeners();
	}

	void Update()
	{
		//glBegin();

		// clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw
		

		// swap buffers
		Storage->Get<StorageWindow>("window")->Window.display();

		//glEnd();
	}

	void Listeners()
	{
		Event->ListenData("WindowResize", [=](void* Size){
			auto sze = *(Event::SizeEvent*)Size;
			glViewport(0, 0, sze.width, sze.height);
		});
	}

};