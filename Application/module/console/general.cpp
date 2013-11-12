#include "module.h"

#include <sfml/Window.hpp>
using namespace std;
using namespace sf;

#include "print.h"


void ModuleConsole::Init()
{
	active = false;
	text = "";
	history_index = 0;

	Callbacks();
	Listeners();
}

void ModuleConsole::Update()
{

}

void ModuleConsole::Listeners()
{
	Event->Listen("SystemInitialized", [=]{
		Entity->Add<Print>(Entity->New())->Text = [=]{ return "Script console: " + text + (active ? "_" : ""); };
	});

	Event->Listen("InputBindConsole", [=]{
		active = !active;
		text = "";
	});

	Event->Listen<char>("WindowKeyText", [=](char Letter){
		if(active)
			text += Letter;
	});

	Event->Listen<Keyboard::Key>("InputKeyPressed", [=](Keyboard::Key Code){
		if(active)
		{
			switch(Code)
			{
			case Keyboard::BackSpace:
				text = text.substr(0, text.size() - 1);
				break;
			case Keyboard::Return:
				if(text.size() < 1) break;
				Script->Inline(text);
				active = false;
				if((history.size() > 0 ? history.back() : "") != text)
					history.push_back(text);
				history_index = history.size();
				text = "";
				break;
			case Keyboard::Up:
				if(history_index > 0) history_index--;
				History();
				break;
			case Keyboard::Down:
				if(history_index < history.size()) history_index++;
				History();
				break;
			}
		}
	});
}
