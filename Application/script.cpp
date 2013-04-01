#pragma once

#include "system.h"
#include "debug.h"

#include <string>
using namespace std;
#include "V8/v8.h"


class ModuleScript : public Module
{
	v8::Persistent<v8::Context> context;
	v8::Persistent<v8::ObjectTemplate> globals;

	void Init()
	{
		v8::Isolate* isolate = v8::Isolate::GetCurrent();
		{
			v8::HandleScope handle_scope(isolate);
			globals = v8::Persistent<v8::ObjectTemplate>::New(isolate, v8::ObjectTemplate::New());
			Register("print", Print);
			context = v8::Context::New(NULL, globals);
			context->Enter();		
		}
	}

	void Register(string Name, function<v8::Handle<v8::Value>(v8::Arguments const &)> Function)
	{
		v8::InvocationCallback* function = Function.target<v8::InvocationCallback>();
		globals->Set(v8::String::New(Name.c_str()), v8::FunctionTemplate::New(*function));
	}

	~ModuleScript()
	{
		context->Exit();
		v8::Isolate* isolate = context->GetIsolate();
		context.Dispose(isolate);
		v8::V8::Dispose();
	}

	void Run(string Source)
	{
		v8::Isolate* isolate = context->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(Source.c_str()));
		v8::Handle<v8::Value> result = script->Run();
	}

	void Update()
	{
		Run("print('Hello World!');");
	}

	static const char* ToCString(const v8::String::Utf8Value& value) {
		return *value ? *value : "<string conversion failed>";
	}

	static v8::Handle<v8::Value> Print(const v8::Arguments& args)
	{
		bool first = true;
		for (int i = 0; i < args.Length(); i++)
		{
			v8::HandleScope handle_scope(args.GetIsolate());
			if (first) first = false;
			else printf(" ");
			v8::String::Utf8Value str(args[i]);
			const char* cstr = ToCString(str);
			printf("%s", cstr);
		}
		printf("\n");
		fflush(stdout);
		return v8::Undefined();
	}
};
