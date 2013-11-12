#include "module.h"

#include <bullet/btBulletDynamicsCommon.h>
using namespace v8;

#include "form.h"


void ModulePhysic::Callbacks()
{
	Script->Bind("impulse",    jsImpulse );
	Script->Bind("force",      jsForce   );
	Script->Bind("velocity",   jsVelocity);
}

Handle<Value> ModulePhysic::jsImpulse(const Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	btVector3 impulse;
	if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
		impulse = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	else if (1 < args.Length() && args[1]->IsNumber())
		impulse = btVector3(0, (float)args[1]->NumberValue(), 0);
	else return Undefined();
	
	frm->Body->activate();
	frm->Body->applyCentralImpulse(impulse);

	return Undefined();
}

Handle<Value> ModulePhysic::jsForce(const Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	btVector3 force;
	if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
		force = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	else if (1 < args.Length() && args[1]->IsNumber())
		force = btVector3(0, (float)args[1]->NumberValue(), 0);
	else return Undefined();
		
	frm->Body->activate();
	frm->Body->applyCentralForce(force);

	return Undefined();
}

Handle<Value> ModulePhysic::jsVelocity(const Arguments& args)
{
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set velocity
	if(args.Length() > 1)
	{

		btVector3 velocity;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			velocity = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
		else if (1 < args.Length() && args[1]->IsNumber())
			velocity = btVector3(0, (float)args[1]->NumberValue(), 0);
		else return Undefined();
		
		frm->Body->activate();
		frm->Body->setLinearVelocity(velocity);
		return Undefined();
	}
	// get velocity
	else
	{
		btVector3 velocity = frm->Body->getLinearVelocity();

		Handle<Array> result = Array::New(3);
		result->Set(0, Number::New(velocity.getX()));
		result->Set(1, Number::New(velocity.getY()));
		result->Set(2, Number::New(velocity.getZ()));
		return result;
	}
}
