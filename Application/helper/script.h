#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include <V8/v8.h>

#include "helper/debug.h"
#include "helper/file.h"


class Module; // better use void* pointer directly

class HelperScript
{
public:
	HelperScript(std::string Name, Module *Module, v8::Persistent<v8::Context> Context) : name(Name), context(Context)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::Local<v8::External> handle = v8::External::New(reinterpret_cast<void*>(Module));
		module = v8::Persistent<v8::External>::New(isolate, handle);
	}
	~HelperScript()
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);
		context->Exit();
	}

	void Bind(std::string Name, std::function<v8::Handle<v8::Value>(v8::Arguments const &)> Function)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::InvocationCallback *function = Function.target<v8::InvocationCallback>();
		v8::Local<v8::Object> global = context->Global();
		global->Set(v8::String::New(Name.c_str()), v8::FunctionTemplate::New(*function, module)->GetFunction(), v8::ReadOnly);
	}

	bool Load(std::string Path)
	{
		if(scripts.find(Path) != scripts.end())
		{
			HelperDebug::Warning("system", "the script (" + Path + ") is already loaded");
			return true;
		}

		std::string source = HelperFile::Read("module/" + name, Path);
		v8::Persistent<v8::Script> script = Compile(source);
		if(script.IsEmpty())
		{
			HelperDebug::Warning("system", "the script (" + Path + ") cannot be compiled");
			return false;
		}
		
		scripts.insert(std::make_pair(Path, script));
		return true;
	}

	v8::Persistent<v8::Value> Run(std::string Path)
	{
		if(scripts.find(Path) == scripts.end())
			if(!Load(Path))
				return v8::Persistent<v8::Value>(v8::Undefined());

		return Execute(scripts[Path]);
	}

	v8::Persistent<v8::Value> Inline(std::string Source)
	{
		v8::Persistent<v8::Script> script = Compile(Source);
		if(script.IsEmpty())
		{
			HelperDebug::Warning("system", "inline script cannot be compiled");
			return v8::Persistent<v8::Value>(v8::Undefined());
		}

		return Execute(script);
	}

	static Module *Unwrap(v8::Local<v8::Value> Data)
	{
		if(Data.IsEmpty())
		{
			HelperDebug::Fail("", "cannot get module from script argument because it is empty");
			return nullptr;
		}
		else if(!Data->IsExternal())
		{
			HelperDebug::Fail("", "cannot get module from script argument because it's a wrong type");
			return nullptr;
		}

		v8::External *handle = v8::External::Cast(*Data);
		return static_cast<Module*>(handle->Value());
	}

private:
	std::string name;
	v8::Persistent<v8::Context> context;
	std::unordered_map<std::string, v8::Persistent<v8::Script> > scripts;
	v8::Persistent<v8::External> module; // this is only for binding scripts

	v8::Persistent<v8::Script> Compile(std::string Source)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(Source.c_str()));
		return v8::Persistent<v8::Script>::New(isolate, script);
	}

	v8::Persistent<v8::Value> Execute(v8::Persistent<v8::Script> Script, std::string Name = "")
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::HandleScope scope(isolate);

		v8::TryCatch trycatch;
		v8::Local<v8::Value> result = Script->Run();
		if(result.IsEmpty())
		{
			v8::Handle<v8::Value> exception = trycatch.Exception();
			v8::String::AsciiValue message(exception);
			if(Name == "")
				HelperDebug::Fail("system", "script crashed:");
			else if(Name == "inline")
				HelperDebug::Fail("system", "inline script crashed:");
			else
				HelperDebug::Fail("system", "script (" + Name + ") crashed:");
			HelperDebug::Inline(std::string(*message) + "\n");
			return v8::Persistent<v8::Value>(v8::Undefined());
		}
		return v8::Persistent<v8::Value>::New(isolate, result);
	}
};
