#include "module.h"

using namespace std;
using namespace v8;


void ModuleConsole::Callbacks()
{
	Script->Bind("print", jsPrint);
	Script->Bind("on",    jsOn);
}

Handle<Value> ModuleConsole::jsPrint(const Arguments& args)
{
	if(args.Length() < 1)
		return Undefined();
	string message = *String::Utf8Value(args[0]);
	HelperDebug::Print("", message);
	return Undefined();
}

Handle<Value> ModuleConsole::jsOn(const Arguments& args)
{
	ModuleConsole *module = (ModuleConsole*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();
	string name = *String::Utf8Value(args[0]);
	
	if(args.Length() < 2 || !args[1]->IsFunction())
		return Undefined();
	Handle<Function> callback = Local<Function>::Cast(args[1]->ToObject());
	
	if(callback.IsEmpty())
	{
		HelperDebug::Fail("script", "invalid callback function");
		return Undefined();
	}

	Persistent<Function> persistent = Persistent<Function>::New(args.GetIsolate(), callback);

	module->Event->Listen(name, [=]{
		HandleScope scope(Isolate::GetCurrent());
		TryCatch trycatch;

		persistent->Call(persistent, 0, NULL);
	});

	return Undefined();
}
