#pragma once

#include "system.h"

#include <SFML/Window.hpp>
using namespace sf;

#include "window.h"
#include "settings.h"


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
				switch(evt.type)
				{
				case Event::KeyPressed:
					Event->Fire("InputKeyPressed", &evt.key.code);
					break;
				case Event::KeyReleased:
					Event->Fire("InputKeyReleased", &evt.key.code);
					break;
				case Event::Closed:
					Close();
					break;
				case Event::Resized:
					Storage->Get<StorageSettings>("settings")->Size = Vector2i(evt.size.width, evt.size.height);
					Event->Fire("WindowResize", &evt.size);
					break;
				}
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
		auto stg = Storage->Get<StorageSettings>("settings");

		VideoMode mde = VideoMode::getDesktopMode();

		stg->Position = Vector2i(mde.width / 2 - stg->Size.x / 2, mde.height / 2 - stg->Size.y / 2);
		Mode(stg->Fullscreen);
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
			stg->Position = wnd->getPosition();
			wnd->create(VideoMode(mde.width, mde.height), stg->Title, Style::Fullscreen, cts);
		}
		else
		{
			wnd->create(VideoMode(stg->Size.x, stg->Size.y), stg->Title, Style::Default, cts);
			wnd->setPosition(stg->Position);
		}

		Event->Fire("WindowCreated", &State);
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