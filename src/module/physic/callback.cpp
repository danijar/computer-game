#include "module.h"

#include <bullet/btBulletDynamicsCommon.h>
using namespace v8;

#include "type/form/type.h"


void ModulePhysic::Callbacks()
{
	Script->Bind("impulse",    jsImpulse );
	Script->Bind("force",      jsForce   );
	Script->Bind("velocity",   jsVelocity);
}

void ModulePhysic::jsImpulse(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = stoull(*String::Utf8Value(args[0]));
	if (!id) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "invalid entity id")));
		return;
	}
	auto frm = module->Entity->Get<Form>(id);
	if (!frm) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "entity has no form property")));
		return;
	}

	btVector3 impulse;
	if (args.Length() > 3) {
		if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be impulse values as numbers")));
			return;
		}
		impulse = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	}
	else if (args.Length() > 1) {
		if (!args[1]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a impulse as number")));
			return;
		}
		impulse = btVector3(0, (float)args[1]->NumberValue(), 0);
	}
	else {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second or second, third and fourth arguments must be impulse values as numbers")));
		return;
	}
	
	frm->Body->activate();
	frm->Body->applyCentralImpulse(impulse);
}

void ModulePhysic::jsForce(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = stoull(*String::Utf8Value(args[0]));
	if (!id) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "invalid entity id")));
		return;
	}
	auto frm = module->Entity->Get<Form>(id);
	if (!frm) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "entity has no form property")));
		return;
	}

	btVector3 force;
	if (args.Length() > 3) {
		if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be force values as numbers")));
			return;
		}
		force = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
	} else if (args.Length() > 1) {
		if (!args[1]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a force as number")));
			return;
		}
		force = btVector3(0, (float)args[1]->NumberValue(), 0);
	}
	else {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second or second, third and fourth arguments must be force values as numbers")));
		return;
	}
		
	frm->Body->activate();
	frm->Body->applyCentralForce(force);
}

void ModulePhysic::jsVelocity(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModulePhysic *module = (ModulePhysic*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = stoull(*String::Utf8Value(args[0]));
	if (!id) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "invalid entity id")));
		return;
	}
	auto frm = module->Entity->Get<Form>(id);
	if (!frm) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "entity has no form property")));
		return;
	}

	// set velocity
	if(args.Length() > 1)
	{

		btVector3 velocity;
		if (args.Length() > 3) {
			if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber())  {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be velocity values as numbers")));
				return;
			}
			velocity = btVector3((float)args[1]->NumberValue(), (float)args[2]->NumberValue(), (float)args[3]->NumberValue());
		} else if (args.Length() > 1) {
			if (!args[1]->IsNumber())  {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a velocity as number")));
				return;
			}
			velocity = btVector3(0, (float)args[1]->NumberValue(), 0);
		}
		else  {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second or second, third and fourth arguments must be velocity values as numbers")));
			return;
		}
		
		frm->Body->activate();
		frm->Body->setLinearVelocity(velocity);
	}
	// get velocity
	else {
		btVector3 velocity = frm->Body->getLinearVelocity();
		Handle<Array> result = Array::New(isolate, 3);
		result->Set(0, Number::New(isolate, velocity.getX()));
		result->Set(1, Number::New(isolate, velocity.getY()));
		result->Set(2, Number::New(isolate, velocity.getZ()));
		args.GetReturnValue().Set(result);
	}
}
