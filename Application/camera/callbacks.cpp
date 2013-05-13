#include "module.h"

#include "settings.h"


void ModuleCamera::Callbacks()
{
	Script->Bind("camera", jsCamera);
	Script->Bind("fov",    jsFov   );
}

v8::Handle<v8::Value> ModuleCamera::jsCamera(const v8::Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	unsigned int id = *module->Global->Get<unsigned int>("camera");

	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleCamera::jsFov(const v8::Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	// set field of view
	if(0 < args.Length())
	{
		if(args[0]->IsNumber())
		{
			stg->Fieldofview = (float)args[0]->NumberValue();
			module->Projection();
			module->Event->Fire("ShaderUpdated");
		}
		return v8::Undefined();
	}
	// get field of view
	else
	{
		//stg->Fieldofview = 
		return v8::Number::New(stg->Fieldofview);
	}
}
