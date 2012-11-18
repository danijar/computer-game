#pragma once

#include "system.h"

#include "window.h"
#include "settings.h"

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
using namespace sf;


class ComponentWindow : public Component
{

	void Init()
	{
		auto wnd = &Storage->Add<StorageWindow>("window")->Window;
		auto stg = Storage->Get<StorageSettings>("settings");

		ContextSettings cts;
		cts.depthBits         = 24;
		cts.stencilBits       =  8;
		cts.antialiasingLevel =  4;
		cts.majorVersion      =  3;
		cts.minorVersion      =  0;

		(*wnd).create(VideoMode(stg->Width, stg->Height), "", Style::Default, cts);

		Center();
		(*wnd).setTitle(stg->Title);

		Listeners();
	}

	void Update()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		
		if((*wnd).isOpen())
		{
			sf::Event evt;
			while ((*wnd).pollEvent(evt))
			{
				if      (evt.type == Event::KeyPressed) { Event->Fire("InputKeyboard", &evt.key.code); }
				else if (evt.type == Event::Closed)     { Close(); }
				else if (evt.type == Event::Resized)    { glViewport(0, 0, evt.size.width, evt.size.height); Event->Fire("WindowResize"); }
			}
		}
	}

	void Listeners()
	{
		Event->ListenData("InputKeyboard", [=](void* Code){
			auto cde = *(Keyboard::Key*)Code;

			if(Keyboard::Key::Escape == cde) Close();
			if(Keyboard::Key::F11    == cde) Fullscreen();
		});
	}

	void Center()
	{
		RECT scr;
		GetWindowRect(GetDesktopWindow(), &scr);

		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		Vector2u sze = (*wnd).getSize();

		(*wnd).setPosition(Vector2i(scr.right / 2 - sze.x / 2, scr.bottom / 2 - sze.y / 2));
	}

	void Fullscreen()
	{
		// toggle fullscreen
		Fullscreen(true);
	}

	void Fullscreen(bool State)
	{
		// set fullscreen
	}

	void Close()
	{
		Event->Fire("WindowClose");

		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		(*wnd).close();

		Exit("The window was closed.");
	}

};