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
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			switch(Code)
			{
			case Keyboard::Key::Space:
				this->Event->Fire("InputBindJump");
				break;
			case Keyboard::Key::Tab:
				this->Event->Fire("InputBindCreate");
				break;
			}
		});
	}

};