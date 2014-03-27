#include "module.h"

using namespace std;
using namespace v8;

void ModuleConsole::Callbacks()
{
	Script->Bind("print",   jsPrint);
	Script->Bind("require", jsRequire);
	Script->Bind("on",      jsOn);
	Script->Bind("remove",  jsRemove);
}

void ModuleConsole::jsPrint(const FunctionCallbackInfo<Value> &args)
{
	if (args.Length() < 1) return;
	string message = *String::Utf8Value(args[0]);
	ManagerLog::Print("", message);
}

void ModuleConsole::jsRequire(const FunctionCallbackInfo<Value> &args)
{
	ModuleConsole *module = (ModuleConsole*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) return;
	string path = *String::Utf8Value(args[0]);
	
	module->Script->Run(path, false);
}

void ModuleConsole::jsOn(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleConsole *module = (ModuleConsole*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be event name as string")));
		return;
	}
	string name = *String::Utf8Value(args[0]);
	
	// fetch callback
	if (args.Length() < 2 || !args[1]->IsFunction()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a function to execute on fire")));
		return;
	}
	Handle<Function> callback = args[1].As<Function>();
	if (callback.IsEmpty()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "callback function is empty")));
		return;
	}

	Persistent<Function, CopyablePersistentTraits<Function>> persistent(isolate, callback);
	module->Event->Listen(name, [=]{
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		TryCatch trycatch;
		Local<Function> function = Local<Function>::New(isolate, persistent);
		function->Call(function, 0, NULL);
	});
}

void ModuleConsole::jsRemove(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleConsole *module = (ModuleConsole*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = stoull(*String::Utf8Value(args[0]));

	module->Entity->Delete(id);
}
