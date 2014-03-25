#include "module.h"

using namespace v8;

#include "type/settings/type.h"
#include "type/camera/type.h"


void ModuleCamera::Callbacks()
{
	Script->Bind("fov",    jsFov   );
	Script->Bind("camera", jsCamera);
}

void ModuleCamera::jsCamera(const FunctionCallbackInfo<Value> &args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	uint64_t id = *module->Global->Get<uint64_t>("camera");

	args.GetReturnValue().Set(String::NewFromUtf8(Isolate::GetCurrent(), to_string(id).c_str()));
}

void ModuleCamera::jsFov(const FunctionCallbackInfo<Value> &args)
{
	Isolate *isolate = Isolate::GetCurrent();
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	// set field of view
	if (0 < args.Length() && args[0]->IsNumber()) {
		stg->Set<float>("Fieldofview", (float)args[0]->NumberValue());
		module->Projection();
		module->Event->Fire("ShaderUpdated");
	}
	// get field of view
	else {
		args.GetReturnValue().Set((double)*stg->Get<float>("Fieldofview"));
	}
}
