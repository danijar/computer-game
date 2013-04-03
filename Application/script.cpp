#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <V8/v8.h>


class ModuleScript : public Module
{
	void Init()
	{
		Script->Bind("print", Print);
		Script->Bind("greet", Greeting);

		Script->Run("init.js");

		Script->Load("update.js");
	}

	void Update()
	{
		Script->Run("update.js");
	}

	static v8::Handle<v8::Value> Greeting(const v8::Arguments& args)
	{
		string introduction = *v8::String::Utf8Value(args[0]);
		Module* module = HelperScript::Unwrap(args.Data());
		string name = module->Name();
		cout << introduction << " " << name << "!" << endl;
		return v8::Undefined();
	}

	static v8::Handle<v8::Value> Print(const v8::Arguments& args)
	{
		bool first = true;
		for (int i = 0; i < args.Length(); ++i)
		{
			v8::HandleScope scope(args.GetIsolate());
			if (first) first = false;
			else cout << " ";
			string message = *v8::String::Utf8Value(args[i]);
			cout << message;
		}
		cout << endl;
		return v8::Undefined();
	}
};
