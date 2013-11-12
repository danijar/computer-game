#include "module.h"

#include <string>
using namespace std;
using namespace v8;

#include "settings.h"


void ModuleInterface::Callbacks()
{
	Script->Bind("interface", jsInterface);
	Script->Bind("debug",     jsDebug    );
}

Handle<Value> ModuleInterface::jsInterface(const Arguments& args)
{
	ModuleInterface *module = (ModuleInterface*)HelperScript::Unwrap(args.Data());

	module->show = !module->show;

	HelperDebug::Print("script", string(module->show ? "enabled" : "disabled") + " interface");
	return Undefined();
}

Handle<Value> ModuleInterface::jsDebug(const Arguments& args)
{
	ModuleInterface *module = (ModuleInterface*)HelperScript::Unwrap(args.Data());

	bool *debug = module->Global->Get<Settings>("settings")->Get<bool>("Debug");
	*debug = !*debug;

	HelperDebug::Print("script", string(*debug ? "enabled" : "disabled") + " debug interface");
	return Undefined();
}
