#pragma once

#include "system.h"

#include <SFML/Window.hpp>
using namespace sf;

class ComponentInput : public Component
{

	void Init()
	{
		Event->ListenData("InputKeyboard", [=](void* Code){
			auto cde = *(Keyboard::Key*)Code;

			// check all keys and bind them to events related to user settings
			//if(Keyboard::Key::Escape == cde){ }
			//else if ...
		});
	}

	void Update()
	{
		//sleep(milliseconds(2000));
	}

};