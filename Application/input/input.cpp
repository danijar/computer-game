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
	Event->Listen<Keyboard::Key>("InputKeyPressed", [=](Keyboard::Key Code){
		switch(Code)
		{
		case Keyboard::Space:
			this->Event->Fire("InputBindJump");
			break;
		case Keyboard::Tab:
			this->Event->Fire("InputBindCreate");
			break;
		}
	});

	Event->Listen<Mouse::Button>("InputMousePressed", [=](Mouse::Button Code){
		switch(Code)
		{
		case Mouse::Left:
			this->Event->Fire("InputBindShoot");
			break;
		}
	});
}
