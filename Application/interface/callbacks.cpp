#include "module.h"

#include <string>
using namespace std;


void ModuleInterface::Callbacks()
{
	Script->Bind("interface", jsInterface);
	Script->Bind("debug",     jsDebug    );
}

v8::Handle<v8::Value> ModuleInterface::jsInterface(const v8::Arguments& args)
{
	ModuleInterface *module = (ModuleInterface*)HelperScript::Unwrap(args.Data());

	module->show = !module->show;

	HelperDebug::Print("script", string(module->show ? "enabled" : "disabled") + " interface");
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleInterface::jsDebug(const v8::Arguments& args)
{
	ModuleInterface *module = (ModuleInterface*)HelperScript::Unwrap(args.Data());

	module->debug = !module->debug;

	HelperDebug::Print("script", string(module->debug ? "enabled" : "disabled") + " debug interface");
	return v8::Undefined();
}
