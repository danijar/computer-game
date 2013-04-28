#include "module.h"

#include <SFML/Window.hpp>
using namespace std;
using namespace sf;

#include "text.h"


void ModuleConsole::Init()
{
	active = false;
	text = "";
	history_index = 0;

	Script->Bind("print", jsPrint);

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

void ModuleConsole::History()
{
	if(history.size() < 1) return;
	
	if(history_index < history.size())
		text = history[history_index];
	else
		text = "";
}

v8::Handle<v8::Value> ModuleConsole::jsPrint(const v8::Arguments& args)
{
	string message = *v8::String::Utf8Value(args[0]);
	HelperDebug::Print("", message);
	return v8::Undefined();
}
