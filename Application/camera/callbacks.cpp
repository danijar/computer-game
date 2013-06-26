#include "module.h"

#include "settings.h"
#include "camera.h"


void ModuleCamera::Callbacks()
{
	Script->Bind("camera", jsCamera);
	Script->Bind("person", jsPerson);
	Script->Bind("fov",    jsFov   );
}

v8::Handle<v8::Value> ModuleCamera::jsCamera(const v8::Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	unsigned int id = *module->Global->Get<unsigned int>("camera");

	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleCamera::jsPerson(const v8::Arguments& args)
{
	ModuleCamera *module = (ModuleCamera*)HelperScript::Unwrap(args.Data());
	auto cam = module->Entity->Get<Camera>(*module->Global->Get<unsigned int>("camera"));
	unsigned int id = cam->Person;

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
			stg->Set<float>("Fieldofview", (float)args[0]->NumberValue());
			module->Projection();
			module->Event->Fire("ShaderUpdated");
		}
		return v8::Undefined();
	}
	// get field of view
	else
	{
		return v8::Number::New(*stg->Get<float>("Fieldofview"));
	}
}
