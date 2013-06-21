#include "module.h"

#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;

#include "settings.h"


void ModuleWindow::Callbacks()
{
	Script->Bind("title", jsTitle);
	Script->Bind("vsync", jsVsync);
	Script->Bind("key",   jsKey  );
}

v8::Handle<v8::Value> ModuleWindow::jsTitle(const v8::Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<RenderWindow>("window");

	if(0 < args.Length())
	{
		if(args[0]->IsString())
		{
			string title = *v8::String::Utf8Value(args[0]);
			stg->Title = title;
			wnd->setTitle(title);
		}
		return v8::Undefined();
	}
	else
	{
		return v8::String::New(stg->Title.c_str());
	}
}

v8::Handle<v8::Value> ModuleWindow::jsVsync(const v8::Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<RenderWindow>("window");

	stg->Verticalsync = !stg->Verticalsync;
	wnd->setVerticalSyncEnabled(stg->Verticalsync);

	HelperDebug::Print("script", string(stg->Verticalsync ? "enabled" : "disabled") + " vertical sync");
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleWindow::jsKey(const v8::Arguments& args)
{
	if(0 < args.Length() && args[0]->IsString())
	{
		string key = *v8::String::Utf8Value(args[0]);
		bool pressed = false;

		if     (key == "A") pressed = Keyboard::isKeyPressed(Keyboard::A);
		else if(key == "B") pressed = Keyboard::isKeyPressed(Keyboard::B);
		else if(key == "C") pressed = Keyboard::isKeyPressed(Keyboard::C);
		else if(key == "D") pressed = Keyboard::isKeyPressed(Keyboard::D);
		else if(key == "E") pressed = Keyboard::isKeyPressed(Keyboard::E);
		else if(key == "F") pressed = Keyboard::isKeyPressed(Keyboard::F);
		else if(key == "G") pressed = Keyboard::isKeyPressed(Keyboard::G);
		else if(key == "H") pressed = Keyboard::isKeyPressed(Keyboard::H);
		else if(key == "I") pressed = Keyboard::isKeyPressed(Keyboard::I);
		else if(key == "J") pressed = Keyboard::isKeyPressed(Keyboard::J);
		else if(key == "K") pressed = Keyboard::isKeyPressed(Keyboard::K);
		else if(key == "L") pressed = Keyboard::isKeyPressed(Keyboard::L);
		else if(key == "M") pressed = Keyboard::isKeyPressed(Keyboard::M);
		else if(key == "N") pressed = Keyboard::isKeyPressed(Keyboard::N);
		else if(key == "O") pressed = Keyboard::isKeyPressed(Keyboard::O);
		else if(key == "P") pressed = Keyboard::isKeyPressed(Keyboard::P);
		else if(key == "Q") pressed = Keyboard::isKeyPressed(Keyboard::Q);
		else if(key == "R") pressed = Keyboard::isKeyPressed(Keyboard::R);
		else if(key == "S") pressed = Keyboard::isKeyPressed(Keyboard::S);
		else if(key == "T") pressed = Keyboard::isKeyPressed(Keyboard::T);
		else if(key == "U") pressed = Keyboard::isKeyPressed(Keyboard::U);
		else if(key == "V") pressed = Keyboard::isKeyPressed(Keyboard::V);
		else if(key == "W") pressed = Keyboard::isKeyPressed(Keyboard::W);
		else if(key == "X") pressed = Keyboard::isKeyPressed(Keyboard::X);
		else if(key == "Y") pressed = Keyboard::isKeyPressed(Keyboard::Y);
		else if(key == "Z") pressed = Keyboard::isKeyPressed(Keyboard::Z);
		else if(key == "0") pressed = Keyboard::isKeyPressed(Keyboard::Num0);
		else if(key == "1") pressed = Keyboard::isKeyPressed(Keyboard::Num1);
		else if(key == "2") pressed = Keyboard::isKeyPressed(Keyboard::Num2);
		else if(key == "3") pressed = Keyboard::isKeyPressed(Keyboard::Num3);
		else if(key == "4") pressed = Keyboard::isKeyPressed(Keyboard::Num4);
		else if(key == "5") pressed = Keyboard::isKeyPressed(Keyboard::Num5);
		else if(key == "6") pressed = Keyboard::isKeyPressed(Keyboard::Num6);
		else if(key == "7") pressed = Keyboard::isKeyPressed(Keyboard::Num7);
		else if(key == "8") pressed = Keyboard::isKeyPressed(Keyboard::Num8);
		else if(key == "9") pressed = Keyboard::isKeyPressed(Keyboard::Num9);
		else if(key == "Shift") pressed = Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift);
		else if(key == "Ctrl" ) pressed = Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl);
		else if(key == "Space") pressed = Keyboard::isKeyPressed(Keyboard::Space);
		else if(key == "Enter") pressed = Keyboard::isKeyPressed(Keyboard::Return);

		return v8::Boolean::New(pressed);
	}
	return v8::Undefined();
}
