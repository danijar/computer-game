#include "module.h"

using namespace std;


v8::Handle<v8::Value> ModuleConsole::jsPrint(const v8::Arguments& args)
{
	string message = *v8::String::Utf8Value(args[0]);
	HelperDebug::Print("", message);
	return v8::Undefined();
}
