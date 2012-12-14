#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/Window.hpp>
using namespace sf;

#include "window.h"
#include "settings.h"


class ComponentWindow : public Component
{

	void Init()
	{
		Storage->Add<StorageWindow>("window");
		auto stg = Storage->Get<StorageSettings>("settings");

		VideoMode mde = VideoMode::getDesktopMode();
		stg->Position = Vector2i(mde.width / 2 - stg->Size.x / 2, mde.height / 2 - stg->Size.y / 2);
		Create(stg->Fullscreen);

		Listeners();
	}

	void Update()
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		auto stg = Storage->Get<StorageSettings>("settings");
		
		if(wnd->isOpen())
		{
			sf::Event evt;
			while (wnd->pollEvent(evt))
			{
				switch(evt.type)
				{
				case Event::KeyPressed:
					Event->Fire<Keyboard::Key>("InputKeyPressed", evt.key.code);
					break;
				case Event::KeyReleased:
					Event->Fire<Keyboard::Key>("InputKeyReleased", evt.key.code);
					break;
				case Event::Closed:
					Close();
					break;
				case Event::Resized:
					Vector2i Size(evt.size.width, evt.size.height);
					Storage->Get<StorageSettings>("settings")->Size = Size;
					Event->Fire<Vector2i>("WindowResize", Size);
					break;
				}
			}
		}

		wnd->setTitle(stg->Title + " (" + to_string(stg->FPS) + " FPS)");
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			switch(Code)
			{
			case Keyboard::Key::Escape:
				Close();
				break;
			case Keyboard::Key::F11:
				Create();
				break;
			}
		});

		Event->Listen("SystemUpdated", [=]{
			auto wnd = &Storage->Get<StorageWindow>("window")->Window;
			wnd->display();
		});
	}

	void Create()
	{
		auto fls = Storage->Get<StorageSettings>("settings")->Fullscreen;
		Create(!fls);
	}

	void Create(bool Fullscreen)
	{
		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		auto stg = Storage->Get<StorageSettings>("settings");

		stg->Fullscreen = Fullscreen;

		bool Recreated = wnd->isOpen();
		VideoMode mde = VideoMode::getDesktopMode();

		ContextSettings cts;
		cts.depthBits         = 24;
		cts.stencilBits       =  8;
		cts.antialiasingLevel =  4;
		cts.majorVersion      =  3;
		cts.minorVersion      =  0;

		if(Fullscreen)
		{
			stg->Position = wnd->getPosition();
			wnd->create(VideoMode(mde.width, mde.height), stg->Title, Style::Fullscreen, cts);
		}
		else
		{
			wnd->create(VideoMode(stg->Size.x, stg->Size.y), stg->Title, Style::Default, cts);
			wnd->setPosition(stg->Position);
		}

		Debug::PassFail("Window creation", wnd->isOpen());

		if(Recreated) Event->Fire("WindowRecreated", &Fullscreen);
	}

	void Close()
	{
		Event->Fire("WindowClose");

		auto wnd = &Storage->Get<StorageWindow>("window")->Window;
		wnd->close();

		Exit("The window was closed.");
	}

};