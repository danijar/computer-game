#pragma once

#include "system.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
using namespace sf;

#include "window.h"
#include "settings.h"


class ComponentRenderer : public Component
{

	void Init()
	{
		auto stg = Storage->Get<StorageSettings>("settings");
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		wnd->setVerticalSyncEnabled(true);

		glewExperimental = GL_TRUE;
		glewInit();

		// testing
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLuint VertexArray, VertexBuffer, ElementBuffer; // save them in storages
		glGenVertexArrays(1, &VertexArray);
		glBindVertexArray(VertexArray);
		glGenBuffers(1, &VertexBuffer);
		glGenBuffers(1, &ElementBuffer);
		glEnable(GL_DEPTH_TEST);

		Listeners();
	}

	void Update()
	{
		// clear
		glClearColor(.4f,.6f,.9f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// testing
		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 1.f, .4f);
		glVertex2f(.2f, .3f);
		glVertex2f(.4f, .5f);
		glVertex2f(.2f, .6f);
		glEnd();

		// swap buffers
		(&Storage->Get<StorageWindow>("window")->Window)->display();
	}

	void Listeners()
	{
		Event->ListenData("WindowResize", [=](void* Size){
			auto sze = *(Event::SizeEvent*)Size;
			glViewport(0, 0, sze.width, sze.height);
		});
	}

};