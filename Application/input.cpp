#pragma once

#include "system.h"

#include <SFML/Window.hpp>
using namespace sf;


class ComponentInput : public Component
{

	void Init()
	{
		Listeners();
	}

	void Update()
	{

	}

	void Listeners()
	{
		Event->ListenData("InputKeyboard", [=](void* Code){
			auto cde = *(Keyboard::Key*)Code;

			// check all keys and bind them to events related to user settings
			//if(Keyboard::Key::Escape == cde){ }
			//else if ...
		});

		Event->ListenData("InputKeyReleased", [=](void* Code){
			auto cde = *(Keyboard::Key*)Code;
			switch(cde)
			{
			case Keyboard::Key::Space:
				this->Event->Fire("InputBindJump");
				break;
			}
		});
	}

};