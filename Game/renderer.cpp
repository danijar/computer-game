#pragma once

#include "system.h"

#include "window.h"

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
using namespace sf;
// include GLEW
// maybe include GLM

class ComponentRenderer : public Component
{

	void Init()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		(*wnd).setVerticalSyncEnabled(true);

		// init GLEW
	}

	void Update()
	{
		//glBegin();

		// clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw
		

		// swap buffers
		Storage->Get<StorageWindow>("window")->Window.display();
		//auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		//(*wnd).display();

		//glEnd();
	}

};