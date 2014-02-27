#include "module.h"

#include <string>
using namespace std;
using namespace v8;

#include "type/settings/type.h"
#include "type/camera/type.h"


void ModulePerson::Callbacks()
{
	Script->Bind("player", jsPlayer);
}

Handle<Value> ModulePerson::jsPlayer(const Arguments& args)
{
	ModulePerson *module = (ModulePerson*)HelperScript::Unwrap(args.Data());
	uint64_t id = *module->Global->Get<uint64_t>("camera");
	if (!id) return Undefined();
	auto cam = module->Entity->Get<Camera>(id);
	if (!cam) return Undefined();

	uint64_t person = cam->Person;
	return String::New(to_string(person).c_str());
}
