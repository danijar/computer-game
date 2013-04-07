#include "module.h"

#include <SFML/Window.hpp>
using namespace sf;


void ModuleInput::Init()
{
	Listeners();
}

void ModuleInput::Update()
{

}

void ModuleInput::Listeners()
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
