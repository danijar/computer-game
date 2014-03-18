#include "module.h"

#include <string>
#include <sfml/Window.hpp>
#include <sfml/Graphics/RenderWindow.hpp>
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
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());

	bool multiple = false;
	sf::Keyboard::Key key;
	vector<sf::Keyboard::Key> keys;
	// get keys from string or array of strings
	if (args.Length() > 0 && args[0]->IsString()) {
		key = module->Key(*String::Utf8Value(args[0]));
	} else if (args.Length() > 0 && args[0]->IsArray()) {
		multiple = true;
		Handle<Array> array = Handle<Array>::Cast(args[0]);
		for (size_t i = 0; i < array->Length(); ++i)
			if (array->Get(i)->IsString())
				keys.push_back(module->Key(*String::Utf8Value(array->Get(i))));
	} else return Undefined();

	// return current key pressed state
	if (args.Length() < 2 || !args[1]->IsFunction()) {
		bool pressed = sf::Keyboard::isKeyPressed(key);
		return Boolean::New(pressed);
	}
	// bind function to key pressed event
	else {
		if (args.Length() > 2 || args[2]->IsFunction()) {
			bool pressed = multiple ? module->Pressed(keys) : sf::Keyboard::isKeyPressed(key);
			return Boolean::New(pressed);
		}

		// fetch callback
		Handle<Function> callback = Local<Function>::Cast(args[1]->ToObject());
		if (callback.IsEmpty()) {
			ManagerLog::Fail("script", "invalid callback function");
			return Undefined();
		}

		// register to key press event
		Persistent<Function> persistent = Persistent<Function>::New(args.GetIsolate(), callback);
		module->Event->Listen<sf::Keyboard::Key>("InputKeyPressed", [=](sf::Keyboard::Key Code) {
			bool trigger = false;
			if (multiple) {
				bool related = (std::find(keys.begin(), keys.end(), Code) != keys.end());
				bool pressed = module->Pressed(keys);
				trigger = related && pressed;
			} else {
				trigger = (Code == key);
			}
			if (trigger) {
				HandleScope scope(Isolate::GetCurrent());
				TryCatch trycatch;
				persistent->Call(persistent, 0, NULL);
			}
		});

		return Undefined();
	}
}
