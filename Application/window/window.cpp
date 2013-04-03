#pragma once

#include "system.h"

#include <GLEW/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "settings.h"
#include "text.h"

class ModuleWindow : public Module
{
	unsigned int window;
	int frames;
	Clock clock;

	void Init()
	{
		Global->Add<RenderWindow>("window");
		auto stg = Global->Get<StorageSettings>("settings");

		VideoMode mde = VideoMode::getDesktopMode();
		stg->Position = Vector2i(mde.width / 2 - stg->Size.x / 2, mde.height / 2 - stg->Size.y / 2);
		Create(stg->Fullscreen);

		frames = 0;
		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			return "FPS " + to_string(stg->FPS);
		};

		Listeners();
	}

	void Update()
	{
		auto wnd = Global->Get<RenderWindow>("window");
		auto stg = Global->Get<StorageSettings>("settings");
		
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
				  { Vector2u Size(evt.size.width, evt.size.height);
					stg->Size = Size;
					Event->Fire<Vector2u>("WindowResize", Size);
					break; }
				case Event::GainedFocus:
					Event->Fire("WindowFocusGained");
					break;
				}
			}
		}

		if(clock.getElapsedTime().asMilliseconds() >= 1000)
		{
			clock.restart();
			stg->FPS = frames;
			frames = 0;
		}
		frames++;
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto wnd = Global->Get<RenderWindow>("window");
			auto stg = Global->Get<StorageSettings>("settings");

			switch(Code)
			{
			case Keyboard::Key::Escape:
				Close();
				break;
			case Keyboard::Key::F2:
				stg->Verticalsync = !stg->Verticalsync;
				wnd->setVerticalSyncEnabled(stg->Verticalsync);
				break;
			case Keyboard::Key::F11:
				Create();
				break;
			}
		});

		Event->Listen("SystemUpdated", [=]{
			Global->Get<RenderWindow>("window")->display();
		});
	}

	void Create()
	{
		auto stg = Global->Get<StorageSettings>("settings");
		Create(!stg->Fullscreen);
	}

	void Create(bool Fullscreen)
	{
		auto wnd = Global->Get<RenderWindow>("window");
		auto stg = Global->Get<StorageSettings>("settings");

		stg->Fullscreen = Fullscreen;

		bool Recreated = wnd->isOpen();
		VideoMode mde = VideoMode::getDesktopMode();

		ContextSettings cts;
		cts.depthBits         = 32;
		cts.stencilBits       =  0;
		cts.antialiasingLevel =  0;
		cts.majorVersion      =  3;
		cts.minorVersion      =  3;

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
		wnd->setVerticalSyncEnabled(stg->Verticalsync);

		Debug->PassFail("creation", wnd->isOpen());

		if(Recreated) Event->Fire<bool>("WindowRecreated", Fullscreen);
	}

	void Close()
	{
		Event->Fire("WindowClose");
		Global->Get<RenderWindow>("window")->close();
		Exit("The window was closed.");
	}

};