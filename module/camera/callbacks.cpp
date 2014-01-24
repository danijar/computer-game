#include "module.h"

using namespace v8;

#include "type/settings/type.h"
#include "type/camera/type.h"


void ModuleCamera::Callbacks()
{
	Script->Bind("fov",    jsFov   );
	Script->Bind("camera", jsCamera);
}

Handle<Value> ModuleCamera::jsCamera(const Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	uint64_t id = *module->Global->Get<uint64_t>("camera");

	return String::New(to_string(id).c_str());
}

Handle<Value> ModuleCamera::jsFov(const Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	// set field of view
	if(0 < args.Length())
	{
		if(args[0]->IsNumber())
		{
			stg->Set<float>("Fieldofview", (float)args[0]->NumberValue());
			module->Projection();
			module->Event->Fire("ShaderUpdated");
		}
		return Undefined();
	}
	// get field of view
	else
	{
		return Number::New(*stg->Get<float>("Fieldofview"));
	}
}
