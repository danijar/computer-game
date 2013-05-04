#include "module.h"

#include <string>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;

#include "transform.h"


void ModuleModel::Callbacks()
{
	Script->Bind("model", jsModel);
	Script->Bind("light", jsLight);
	Script->Bind("getposition", jsGetPosition);
	Script->Bind("setposition", jsSetPosition);
	Script->Bind("getrotation", jsGetRotation);
	Script->Bind("setrotation", jsSetRotation);
}

v8::Handle<v8::Value> ModuleModel::jsModel(const v8::Arguments& args) // rotation get passed in degrees from script
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	string mesh = *v8::String::Utf8Value(args[0]);
	string material = *v8::String::Utf8Value(args[1]);
	vec3 position(args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue());
	vec3 rotation(args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue());
	vec3 scale(args[8]->NumberValue());
	float mass = (float)args[9]->NumberValue();

	unsigned int id = module->Model(mesh, material, position, radians(rotation), scale, mass);
	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleModel::jsLight(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	vec3 position(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
	float radius = (float)args[3]->NumberValue();
	vec3 color(args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue());
	float intensity  = (float)args[7]->NumberValue();

	unsigned int id = module->Light(position, radius, color, intensity, StorageLight::POINT);
	return v8::Uint32::New(id);
}

v8::Handle<v8::Value> ModuleModel::jsGetPosition(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();

	auto tsf = module->Entity->Get<StorageTransform>(id);
	vec3 position = tsf->Position();

	v8::Handle<v8::Array> result = v8::Array::New(3);
	result->Set(0, v8::Number::New(position.x));
	result->Set(1, v8::Number::New(position.y));
	result->Set(2, v8::Number::New(position.z));
	return result;
}

v8::Handle<v8::Value> ModuleModel::jsSetPosition(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();
	vec3 position(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());

	auto tsf = module->Entity->Get<StorageTransform>(id);
	tsf->Position(position);
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleModel::jsGetRotation(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();

	auto tsf = module->Entity->Get<StorageTransform>(id);
	vec3 rotation = tsf->Rotation();

	v8::Handle<v8::Array> result = v8::Array::New(3);
	result->Set(0, v8::Number::New(rotation.x));
	result->Set(1, v8::Number::New(rotation.y));
	result->Set(2, v8::Number::New(rotation.z));
	return result;
}

v8::Handle<v8::Value> ModuleModel::jsSetRotation(const v8::Arguments& args)
{
	ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	unsigned int id = args[0]->Uint32Value();
	vec3 rotation(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());

	auto tsf = module->Entity->Get<StorageTransform>(id);
	tsf->Rotation(rotation);
	return v8::Undefined();
}
