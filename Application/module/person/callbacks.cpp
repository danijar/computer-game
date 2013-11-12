#include "module.h"

#include <string>
using namespace std;
using namespace v8;

#include "settings.h"
#include "camera.h"


void ModulePerson::Callbacks()
{
	Script->Bind("player", jsPlayer);
}

Handle<Value> ModulePerson::jsPlayer(const Arguments& args)
{
	ModulePerson *module = (ModulePerson*)HelperScript::Unwrap(args.Data());
	auto cam = module->Entity->Get<Camera>(*module->Global->Get<uint64_t>("camera"));
	uint64_t id = cam->Person;

	return String::New(to_string(id).c_str());
}
