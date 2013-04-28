#include "module.h"

#include <SFML/Window.hpp>
using namespace sf;

#include "text.h"


void ModuleConsole::Init()
{
	text = "";
	active = false;

	Listeners();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{ return "Script console: " + text + (active ? "_" : ""); };
}

void ModuleConsole::Update()
{

}

void ModuleConsole::Listeners()
{
	Event->Listen("InputBindConsole", [=]{
		active = !active;
		text = "";
	});

	Event->Listen<char>("WindowKeyText", [=](char Letter){
		if(active)
			text += Letter;
	});

	Event->Listen<Keyboard::Key>("InputKeyPressed", [=](Keyboard::Key Code){
		switch(Code)
		{
		case Keyboard::BackSpace:
			text = text.substr(0, text.size() - 1);
			break;
		case Keyboard::Return:
			Debug->Print("run script (" + text + ")");
			Script->Inline(text);
			active = false;
			text = "";
			break;
		}
	});
}
