#include "module.h"

using namespace std;


void ModuleConsole::Callbacks()
{
	Script->Bind("print", jsPrint);
	Script->Bind("on",    jsOn);
}

v8::Handle<v8::Value> ModuleConsole::jsPrint(const v8::Arguments& args)
{
	if(args.Length() < 1)
		return v8::Undefined();
	string message = *v8::String::Utf8Value(args[0]);
	HelperDebug::Print("", message);
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleConsole::jsOn(const v8::Arguments& args)
{
	ModuleConsole *module = (ModuleConsole*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return v8::Undefined();
	string name = *v8::String::Utf8Value(args[0]);
	
	if(args.Length() < 2 || !args[1]->IsFunction())
		return v8::Undefined();
	v8::Handle<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]->ToObject());
	
	if(callback.IsEmpty())
	{
		HelperDebug::Fail("script", "invalid callback function");
		return v8::Undefined();
	}

	v8::Persistent<v8::Function> persistent = v8::Persistent<v8::Function>::New(args.GetIsolate(), callback);

	module->Event->Listen(name, [=]{
		v8::HandleScope scope(v8::Isolate::GetCurrent());
		v8::TryCatch trycatch;

		persistent->Call(persistent, 0, NULL);
	});

	return v8::Undefined();
}
