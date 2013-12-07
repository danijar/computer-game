#include "module.h"

#include <string>
#include "dependency/sfml/Window.hpp"
#include "dependency/sfml/Graphics/RenderWindow.hpp"
using namespace std;
using namespace v8;

#include "type/settings/type.h"


void ModuleWindow::Callbacks()
{
	Script->Bind("title", jsTitle);
	Script->Bind("vsync", jsVsync);
	Script->Bind("key",   jsKey  );
}

Handle<Value> ModuleWindow::jsTitle(const Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	if(0 < args.Length())
	{
		if(args[0]->IsString())
		{
			string title = *String::Utf8Value(args[0]);
			stg->Set<string>("Title", title);
			wnd->setTitle(title);
		}
		return Undefined();
	}
	else
	{
		return String::New(stg->Get<string>("Title")->c_str());
	}
}

Handle<Value> ModuleWindow::jsVsync(const Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	stg->Set<bool>("Verticalsync", !*stg->Get<bool>("Verticalsync"));
	wnd->setVerticalSyncEnabled(*stg->Get<bool>("Verticalsync"));

	ManagerLog::Print("script", string(*stg->Get<bool>("Verticalsync") ? "enabled" : "disabled") + " vertical sync");
	return Undefined();
}

Handle<Value> ModuleWindow::jsKey(const Arguments& args)
{
	if(0 < args.Length() && args[0]->IsString())
	{
		string key = *String::Utf8Value(args[0]);
		bool pressed = false;

		if     (key == "A") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		else if(key == "B") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::B);
		else if(key == "C") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
		else if(key == "D") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		else if(key == "E") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
		else if(key == "F") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
		else if(key == "G") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::G);
		else if(key == "H") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
		else if(key == "I") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::I);
		else if(key == "J") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::J);
		else if(key == "K") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
		else if(key == "L") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
		else if(key == "M") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::M);
		else if(key == "N") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::N);
		else if(key == "O") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::O);
		else if(key == "P") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
		else if(key == "Q") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
		else if(key == "R") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
		else if(key == "S") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
		else if(key == "T") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::T);
		else if(key == "U") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::U);
		else if(key == "V") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::V);
		else if(key == "W") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
		else if(key == "X") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
		else if(key == "Y") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Y);
		else if(key == "Z") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
		else if(key == "0") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num0);
		else if(key == "1") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
		else if(key == "2") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
		else if(key == "3") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
		else if(key == "4") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
		else if(key == "5") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num5);
		else if(key == "6") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num6);
		else if(key == "7") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num7);
		else if(key == "8") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num8);
		else if(key == "9") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Num9);
		else if(key == "Shift") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		else if(key == "Ctrl" ) pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
		else if(key == "Space") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		else if(key == "Enter") pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);

		return Boolean::New(pressed);
	}
	return Undefined();
}
