#include "module.h"

#include <string>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;

#include "terrain.h"
#include "model.h"


void ModuleInterface::Callbacks()
{
	Script->Bind("interface", jsInterface);
}

v8::Handle<v8::Value> ModuleInterface::jsInterface(const v8::Arguments& args)
{
	ModuleInterface *module = (ModuleInterface*)HelperScript::Unwrap(args.Data());

	module->show = !module->show;

	HelperDebug::Print("script", string(module->show ? "enabled" : "disabled") + " interface");
	return v8::Undefined();
}
