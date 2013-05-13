#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>

#include "form.h"


void ModulePhysic::Callbacks()
{
	Script->Bind("impulse", jsImpulse);
}

v8::Handle<v8::Value> ModulePhysic::jsImpulse(const v8::Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data()); // do this cast in system.h as template

	if(!args[0]->IsUint32())
		return v8::Undefined();
	unsigned int id = args[0]->Uint32Value();
	auto tsf = module->Entity->Get<Form>(id);

	btVector3 impulse;
	if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
		impulse = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	else if (1 < args.Length() && args[1]->IsNumber())
		impulse = btVector3(0, (float)args[1]->NumberValue(), 0);
	else return v8::Undefined();
		
	tsf->Body->applyCentralImpulse(impulse);

	return v8::Undefined();
}
