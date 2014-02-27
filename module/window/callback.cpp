#include "module.h"

#include <string>
#include <dependency/sfml/Window.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
using namespace std;
using namespace v8;

#include "type/settings/type.h"


void ModuleWindow::Callbacks()
{
	Script->Bind("title",      jsTitle     );
	Script->Bind("vsync",      jsVsync     );
	Script->Bind("windowsize", jsWindowsize);
	Script->Bind("key",        jsKey       );
}

/*
 * title(string?)
 * Set or get window title.
 */
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

/* 
 * vsync()
 * Toggles Vsync.
 */
Handle<Value> ModuleWindow::jsVsync(const Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	stg->Set<bool>("Verticalsync", !stg->Is("Verticalsync"));
	wnd->setVerticalSyncEnabled(stg->Is("Verticalsync"));

	ManagerLog::Print("script", string(stg->Is("Verticalsync") ? "enabled" : "disabled") + " vertical sync");
	return Undefined();
}

/*
 * windowsize()
 * Get dimensions of the window. Return value is an array of length two.
 * Later on also allow to set the window size.
 */
Handle<Value> ModuleWindow::jsWindowsize(const Arguments& args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	sf::Vector2u size = module->Global->Get<sf::RenderWindow>("window")->getSize();

	Handle<Array> result = Array::New(2);
	result->Set(0, Number::New(size.x));
	result->Set(1, Number::New(size.y));
	return result;
}

/*
 * key(character)
 * Checks whether the key corresponding to the passed letter is currently pressed.
 */
Handle<Value> ModuleWindow::jsKey(const Arguments& args)
{
	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();

	string name = *String::Utf8Value(args[0]);
	sf::Keyboard::Key key;

	if     (name == "A") key = sf::Keyboard::A;
	else if(name == "B") key = sf::Keyboard::B;
	else if(name == "C") key = sf::Keyboard::C;
	else if(name == "D") key = sf::Keyboard::D;
	else if(name == "E") key = sf::Keyboard::E;
	else if(name == "F") key = sf::Keyboard::F;
	else if(name == "G") key = sf::Keyboard::G;
	else if(name == "H") key = sf::Keyboard::H;
	else if(name == "I") key = sf::Keyboard::I;
	else if(name == "J") key = sf::Keyboard::J;
	else if(name == "K") key = sf::Keyboard::K;
	else if(name == "L") key = sf::Keyboard::L;
	else if(name == "M") key = sf::Keyboard::M;
	else if(name == "N") key = sf::Keyboard::N;
	else if(name == "O") key = sf::Keyboard::O;
	else if(name == "P") key = sf::Keyboard::P;
	else if(name == "Q") key = sf::Keyboard::Q;
	else if(name == "R") key = sf::Keyboard::R;
	else if(name == "S") key = sf::Keyboard::S;
	else if(name == "T") key = sf::Keyboard::T;
	else if(name == "U") key = sf::Keyboard::U;
	else if(name == "V") key = sf::Keyboard::V;
	else if(name == "W") key = sf::Keyboard::W;
	else if(name == "X") key = sf::Keyboard::X;
	else if(name == "Y") key = sf::Keyboard::Y;
	else if(name == "Z") key = sf::Keyboard::Z;
	else if(name == "0") key = sf::Keyboard::Num0;
	else if(name == "1") key = sf::Keyboard::Num1;
	else if(name == "2") key = sf::Keyboard::Num2;
	else if(name == "3") key = sf::Keyboard::Num3;
	else if(name == "4") key = sf::Keyboard::Num4;
	else if(name == "5") key = sf::Keyboard::Num5;
	else if(name == "6") key = sf::Keyboard::Num6;
	else if(name == "7") key = sf::Keyboard::Num7;
	else if(name == "8") key = sf::Keyboard::Num8;
	else if(name == "9") key = sf::Keyboard::Num9;
	else if(name == "Shift") key = sf::Keyboard::LShift;
	else if(name == "Ctrl" ) key = sf::Keyboard::LControl;
	else if(name == "Space") key = sf::Keyboard::Space;
	else if(name == "Enter") key = sf::Keyboard::Return;

	// return current key pressed state
	if(args.Length() < 2 || !args[1]->IsFunction()) {
		bool pressed = sf::Keyboard::isKeyPressed(key);
		return Boolean::New(pressed);
	}
	// bind function to key pressed event
	else {
		ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());

		Handle<Function> callback = Local<Function>::Cast(args[1]->ToObject());
		if(callback.IsEmpty())
		{
			ManagerLog::Fail("script", "invalid callback function");
			return Undefined();
		}

		Persistent<Function> persistent = Persistent<Function>::New(args.GetIsolate(), callback);

		module->Event->Listen<sf::Keyboard::Key>("InputKeyPressed", [=](sf::Keyboard::Key Code){
			if (Code == key) {
				HandleScope scope(Isolate::GetCurrent());
				TryCatch trycatch;
				persistent->Call(persistent, 0, NULL);
			}
		});

		return Undefined();
	}
}
