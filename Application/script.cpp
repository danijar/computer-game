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

		Script->Load("init.js", "var number = 42;");
		Script->Run("init.js");

		string source = " number++;                           "
		                " var message = '';                   "
		                " message += 'The sense of life is '; "
		                " message += number;                  "
		                " message += '.';                     "
		                " print('Hello', 'World!', message);  ";
		Script->Load("update.js", source);
	}

	void Update()
	{
		Script->Run("update.js");
	}

	static v8::Handle<v8::Value> Print(const v8::Arguments& args)
	{
		bool first = true;
		for (int i = 0; i < args.Length(); ++i)
		{
			v8::HandleScope scope(args.GetIsolate());
			if (first) first = false;
			else cout << " ";
			string message = string(*v8::String::Utf8Value(args[i]));
			cout << message;
		}
		cout << endl;
		return v8::Undefined();
	}
};
