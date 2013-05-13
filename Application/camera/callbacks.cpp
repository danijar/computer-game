#include "module.h"


void ModuleCamera::Callbacks()
{
	Script->Bind("camera", jsCamera);
}

v8::Handle<v8::Value> ModuleCamera::jsCamera(const v8::Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());

	unsigned int id = *module->Global->Get<unsigned int>("camera");
	return v8::Uint32::New(id);
}
