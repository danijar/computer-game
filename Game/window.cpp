#pragma once

#include "system.h"

#include "window.h"
#include "settings.h"

#include <SFML/Window.hpp>
using namespace sf;


class ComponentWindow : public Component
{

	void Init()
	{
		Create();
		Listeners();
	}

	void Update()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		
		if(wnd->isOpen())
		{
			sf::Event evt;
			while (wnd->pollEvent(evt))
			{
				if      (evt.type == Event::KeyPressed)  { Event->Fire("InputKeyPressed", &evt.key.code); }
				else if (evt.type == Event::KeyReleased) { Event->Fire("InputKeyReleased", &evt.key.code); }
				else if (evt.type == Event::Closed)      { Close(); }
				else if (evt.type == Event::Resized)     { Event->Fire("WindowResize", &evt.size); }
			}
		}
	}

	void Listeners()
	{
		Event->ListenData("InputKeyReleased", [=](void* Code){
			auto cde = *(Keyboard::Key*)Code;
			if(Keyboard::Key::Escape == cde) Close();
			if(Keyboard::Key::F11    == cde) Mode();
		});
	}

	void Create()
	{
		Storage->Add<StorageWindow>("window");

		auto fls = Storage->Get<StorageSettings>("settings")->Fullscreen;
		Mode(fls);
	}

	void Mode()
	{
		auto fls = Storage->Get<StorageSettings>("settings")->Fullscreen;
		Mode(!fls);
	}

	void Mode(bool State) // State(true:fullscreen, false:windowed)
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		auto stg = Storage->Get<StorageSettings>("settings");

		stg->Fullscreen = State;

		VideoMode mde = VideoMode::getDesktopMode();

		ContextSettings cts;
		cts.depthBits         = 24;
		cts.stencilBits       =  8;
		cts.antialiasingLevel =  4;
		cts.majorVersion      =  3;
		cts.minorVersion      =  0;

		if(State)
		{
			wnd->create(VideoMode(mde.width, mde.height), stg->Title, Style::Fullscreen, cts);
		}
		else
		{
			wnd->create(VideoMode(stg->Width, stg->Height), stg->Title, Style::Default, cts);
			wnd->setPosition(Vector2i(mde.width / 2 - stg->Width / 2, mde.height / 2 - stg->Height / 2));
		}
	}

	void Close()
	{
		Event->Fire("WindowClose");

		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		wnd->close();

		Exit("The window was closed.");
	}

};


// fps counter
// auto tle = Storage->Get<StorageSettings>("settings")->Title;
// auto wnd &Storage->Get<StorageWindow>("window")->Window
// wnd->setTitle(tle + " 60 FPS");