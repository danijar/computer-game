#include "module.h"

#include <dependency/sfml/Window.hpp>
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
		case Keyboard::F:
			this->Event->Fire("InputBindShoot");
			break;
		}
	});
	
	Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
		switch(Code)
		{
		case Keyboard::Escape:
			this->Event->Fire("InputBindExit");
			break;
		case Keyboard::Key::F1:
			this->Event->Fire("InputBindCamera");
			break;
		case Keyboard::Key::F2:
			this->Event->Fire("InputBindWireframe");
			break;
		case Keyboard::Key::F3:
			this->Event->Fire("InputBindDebugdraw");
			break;
		case Keyboard::Key::F4:
			this->Event->Fire("InputBindVsync");
			break;
		case Keyboard::Key::F11:
			this->Event->Fire("InputBindFullscreen");
			break;
		case Keyboard::Tab:
			this->Event->Fire("InputBindConsole");
			break;
		}
	});

	Event->Listen<Mouse::Button>("InputMousePressed", [=](Mouse::Button Code){
		switch(Code)
		{
		case Mouse::Left:
			this->Event->Fire("InputBindMine");
			break;
		case Mouse::Right:
			this->Event->Fire("InputBindPlace");
			break;
		case Mouse::Middle:
			this->Event->Fire("InputBindPick");
			break;
		}
	});
}
