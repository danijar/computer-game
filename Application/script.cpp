#pragma once

#include "system.h"
#include "debug.h"

#include <string>
using namespace std;
#include "V8/v8.h"


class ModuleScript : public Module
{
	void Init()
	{
		Script->Bind("debug", Debug);

		string source = " var number = 42;                    "
		                " var message = '';                   "
		                " message += 'The sense of life is '; "
		                " message += number;                  "
		                " message += '.';                     "
		                " debug('Hello', 'World!', message);  ";

		Script->Load("update.js", source);
	}

	void Update()
	{
		Script->Run("update.js");
	}

	static v8::Handle<v8::Value> Debug(const v8::Arguments& args)
	{
		bool first = true;
		for (int i = 0; i < args.Length(); ++i)
		{
			v8::HandleScope scope(args.GetIsolate());
			if (first) first = false;
			else Debug::Inline(" ");
			string message = string(*v8::String::Utf8Value(args[i]));
			Debug::Inline(message);
		}
		Debug::Info("");
		return v8::Undefined();
	}
};
