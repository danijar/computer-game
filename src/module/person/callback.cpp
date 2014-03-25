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

void ModulePerson::jsPlayer(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);

	ModulePerson *module = (ModulePerson*)HelperScript::Unwrap(args.Data());
	uint64_t id = *module->Global->Get<uint64_t>("camera");
	if (!id) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "camera is not initialized yet")));
		return;
	}
	auto cam = module->Entity->Get<Camera>(id);
	if (!cam) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "camera is not initialized yet")));
		return;
	}

	uint64_t person = cam->Person;
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, to_string(person).c_str()));
}
