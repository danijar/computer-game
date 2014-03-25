#include "manager.h"

#include "manager/log/manager.h"
#include "manager/file/manager.h"

using namespace std;
using namespace v8;

HelperScript::HelperScript(string Name, Module *Module, Persistent<Context> Context) : name(Name), global(Context) // make context static instead of passing it
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	Local<External> external = External::New(isolate, reinterpret_cast<void*>(Module));
	module = Persistent<External>(isolate, external);
}
HelperScript::~HelperScript()
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	Local<Context>::New(isolate, global)->Exit();
}

void HelperScript::Bind(string Name, function<void(FunctionCallbackInfo<Value> const &)> Function)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	FunctionCallback *function = Function.target<FunctionCallback>();
	Local<Object> global = Local<Context>::New(isolate, global)->Global();
	global->Set(String::NewFromUtf8(isolate, Name.c_str()), FunctionTemplate::New(isolate, *function, Local<External>::New(isolate, module))->GetFunction(), ReadOnly);
}

bool HelperScript::Load(string Path, bool Module = true)
{
	if (scripts.find(Path) != scripts.end()) {
		ManagerLog::Warning("system", "the script (" + Path + ") is already loaded");
		return true;
	}

	string path = Module ? "module/" + name + "/" + Path : Path;
	string source = HelperFile::Read(name, path);
	Persistent<Script> script = Compile(source);
	if (script.IsEmpty()) {
		ManagerLog::Warning("system", "the script (" + Path + ") cannot be compiled");
		return false;
	}
		
	scripts.insert(make_pair(Path, script));
	return true;
}

Persistent<Value> HelperScript::Run(string Path, bool Module = true)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	if (scripts.find(Path) == scripts.end())
		if (!Load(Path, Module))
			return Persistent<Value>(isolate, Undefined(isolate));

	return Execute(scripts[Path]);
}

Persistent<Value> HelperScript::Inline(string Source)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	Persistent<Script> script = Compile(Source);
	if (script.IsEmpty()) {
		ManagerLog::Warning("system", "inline script cannot be compiled");
		return Persistent<Value>(isolate, Undefined(isolate));
	}

	return Execute(script);
}

Module *HelperScript::Unwrap(Local<Value> Data)
{
	if (Data.IsEmpty()) {
		ManagerLog::Fail("", "cannot get module from script argument because it is empty");
		return nullptr;
	} else if(!Data->IsExternal()) {
		ManagerLog::Fail("", "cannot get module from script argument because it's a wrong type");
		return nullptr;
	}

	External *handle = External::Cast(*Data);
	return static_cast<Module*>(handle->Value());
}

Persistent<Script> HelperScript::Compile(string Source)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	Handle<Script> script = Script::Compile(String::NewFromUtf8(isolate, Source.c_str()));
	return Persistent<Script>(isolate, script);
}

Persistent<Value> HelperScript::Execute(Persistent<Script> Script, string Name)
{
	Isolate *isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	TryCatch trycatch;
	Local<Value> result = Local<v8::Script>::New(isolate, Script)->Run();
	if(result.IsEmpty())
	{
		Handle<Value> exception = trycatch.Exception();
		String::Utf8Value message(exception);
		if(Name == "")
			ManagerLog::Fail("system", "script crashed:");
		else if(Name == "inline")
			ManagerLog::Fail("system", "inline script crashed:");
		else
			ManagerLog::Fail("system", "script (" + Name + ") crashed:");
		ManagerLog::Inline(string(*message) + "\n");
		return Persistent<Value>(isolate, Undefined(isolate));
	}
	return Persistent<Value>(isolate, result);
}
