#include "module.h"

#include <bullet/btBulletDynamicsCommon.h>

#include "form.h"


void ModulePhysic::Callbacks()
{
	Script->Bind("impulse",    jsImpulse );
	Script->Bind("force",      jsForce   );
	Script->Bind("velocity",   jsVelocity);
}

v8::Handle<v8::Value> ModulePhysic::jsImpulse(const v8::Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsUint32())
		return v8::Undefined();
	uint64_t id = args[0]->Uint32Value();
	auto frm = module->Entity->Get<Form>(id);

	btVector3 impulse;
	if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
		impulse = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	else if (1 < args.Length() && args[1]->IsNumber())
		impulse = btVector3(0, (float)args[1]->NumberValue(), 0);
	else return v8::Undefined();
	
	frm->Body->activate();
	frm->Body->applyCentralImpulse(impulse);

	return v8::Undefined();
}

v8::Handle<v8::Value> ModulePhysic::jsForce(const v8::Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsUint32())
		return v8::Undefined();
	uint64_t id = args[0]->Uint32Value();
	auto frm = module->Entity->Get<Form>(id);

	btVector3 force;
	if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
		force = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	else if (1 < args.Length() && args[1]->IsNumber())
		force = btVector3(0, (float)args[1]->NumberValue(), 0);
	else return v8::Undefined();
		
	frm->Body->activate();
	frm->Body->applyCentralForce(force);

	return v8::Undefined();
}

v8::Handle<v8::Value> ModulePhysic::jsVelocity(const v8::Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsUint32())
		return v8::Undefined();
	uint64_t id = args[0]->Uint32Value();
	auto frm = module->Entity->Get<Form>(id);

	// set velocity
	if(args.Length() > 1)
	{

		btVector3 velocity;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			velocity = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
		else if (1 < args.Length() && args[1]->IsNumber())
			velocity = btVector3(0, (float)args[1]->NumberValue(), 0);
		else return v8::Undefined();
		
		frm->Body->activate();
		frm->Body->setLinearVelocity(velocity);
		return v8::Undefined();
	}
	// get velocity
	else
	{
		btVector3 velocity = frm->Body->getLinearVelocity();

		v8::Handle<v8::Array> result = v8::Array::New(3);
		result->Set(0, v8::Number::New(velocity.getX()));
		result->Set(1, v8::Number::New(velocity.getY()));
		result->Set(2, v8::Number::New(velocity.getZ()));
		return result;
	}
}
