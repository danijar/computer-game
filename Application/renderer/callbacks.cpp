#include "module.h"

#include <string>
using namespace std;

#include "settings.h"


void ModuleRenderer::Callbacks()
{
	Script->Bind("wireframe", jsWireframe);
}

v8::Handle<v8::Value> ModuleRenderer::jsWireframe(const v8::Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	stg->Wireframe = !stg->Wireframe;

	HelperDebug::Print("script", string(stg->Wireframe ? "enabled" : "disabled") + " wireframe mode");
	return v8::Undefined();
}
