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
void ModuleWindow::jsTitle(const FunctionCallbackInfo<Value> &args)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	// set title
	if(args.Length() > 0) {
		if (!args[0]->IsString()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be title as string")));
			return;
		} else {
			string title = *String::Utf8Value(args[0]);
			stg->Set<string>("Title", title);
			wnd->setTitle(title);
		}
	}
	// get title
	else {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, stg->Get<string>("Title")->c_str()));
	}
}

/* 
 * vsync()
 * Toggles Vsync.
 */
void ModuleWindow::jsVsync(const FunctionCallbackInfo<Value> &args)
{
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	stg->Set<bool>("Verticalsync", !stg->Is("Verticalsync"));
	wnd->setVerticalSyncEnabled(stg->Is("Verticalsync"));

	ManagerLog::Print("script", string(stg->Is("Verticalsync") ? "enabled" : "disabled") + " vertical sync");
}

/*
 * windowsize()
 * Get dimensions of the window. Return value is an array of length two.
 * Later on also allow to set the window size.
 */
void ModuleWindow::jsWindowsize(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());
	auto wnd = module->Global->Get<sf::RenderWindow>("window");

	sf::Vector2u size = module->Global->Get<sf::RenderWindow>("window")->getSize();

	Handle<Array> result = Array::New(isolate, 2);
	result->Set(0, Number::New(isolate, size.x));
	result->Set(1, Number::New(isolate, size.y));
	args.GetReturnValue().Set(result);
}

/*
 * key(character | array of characters, callback?)
 * Checks whether the key corresponding to the passed letter is currently pressed.
 * When passing an array of letters, it gets checked whether all of those are
 * currently pressed. When a callback is passed, it gets registered to the key
 * press event of the key or keys instead.
 */
void ModuleWindow::jsKey(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleWindow *module = (ModuleWindow*)HelperScript::Unwrap(args.Data());

	// get keys from string or array of strings
	bool multiple = false;
	sf::Keyboard::Key key;
	vector<sf::Keyboard::Key> keys;
	if (args.Length() > 0 && args[0]->IsString()) {
		key = module->Key(*String::Utf8Value(args[0]));
	} else if (args.Length() > 0 && args[0]->IsArray()) {
		multiple = true;
		Handle<Array> array = Handle<Array>::Cast(args[0]);
		for (size_t i = 0; i < array->Length(); ++i)
			if (array->Get(i)->IsString())
				keys.push_back(module->Key(*String::Utf8Value(array->Get(i))));
	} else {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be key as string or array of that")));
		return;
	}

	// bind function to key pressed event
	if (args.Length() > 1) {

		// fetch callback
		if (!args[1]->IsFunction()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a function to execute on key press")));
			return;
		}
		Handle<Function> callback = args[1].As<Function>();
		if (callback.IsEmpty()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "callback function is empty")));
			return;
		}

		// register to key press event
		Persistent<Function, CopyablePersistentTraits<Function>> persistent(isolate, callback);

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
				TryCatch trycatch;
				Local<Function> function = Local<Function>::New(Isolate::GetCurrent(), persistent);
				function->Call(function, 0, NULL);
			}
		});
	}
	// return current key pressed state
	else {
		bool pressed = multiple ? module->Pressed(keys) : sf::Keyboard::isKeyPressed(key);
		args.GetReturnValue().Set(pressed);
	}
}
