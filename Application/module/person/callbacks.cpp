#include "module.h"

#include "settings.h"
#include "camera.h"


void ModulePerson::Callbacks()
{
	Script->Bind("player", jsPlayer);
}

v8::Handle<v8::Value> ModulePerson::jsPlayer(const v8::Arguments& args)
{
	ModulePerson *module = (ModulePerson*)HelperScript::Unwrap(args.Data());
	auto cam = module->Entity->Get<Camera>(*module->Global->Get<uint64_t>("camera"));
	uint64_t id = cam->Person;

	return v8::Uint32::New(id);
}
