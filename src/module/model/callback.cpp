#include "module.h"

#include <string>
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
using namespace v8;

#include "type/form/type.h"

void ModuleModel::Callbacks()
{
	Script->Bind("model",     jsModel    );
	Script->Bind("light",     jsLight    );
	Script->Bind("position",  jsPosition );
	Script->Bind("rotation",  jsRotation );
	Script->Bind("scale",     jsScale    );
	Script->Bind("direction", jsDirection);
	Script->Bind("radius",    jsRadius   );
	Script->Bind("intensity", jsIntensity);
	Script->Bind("color",     jsColor    );
}

void ModuleModel::jsModel(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first and second arguments must be paths to mesh and material as strings")));
		return;
	}
	string mesh = *String::Utf8Value(args[0]);
	string material = *String::Utf8Value(args[1]);

	if (args.Length() < 5 || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "third, fourth and fifth arguments must be position coordinates as numbers")));
		return;
	}
	vec3 position(args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue());

	vec3 rotation; // rotation is passed in degrees from script
	if (7 < args.Length() && args[5]->IsNumber() && args[6]->IsNumber() && args[7]->IsNumber())
		rotation = vec3(args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue());

	vec3 scale(1);
	float mass = 0;
	if (10 < args.Length() && args[8]->IsNumber() && args[9]->IsNumber() && args[10]->IsNumber() && args[11]->IsNumber()) {
		scale = vec3(args[8]->NumberValue(), args[9]->NumberValue(), args[10]->NumberValue());
		mass = (float)args[11]->NumberValue();
	} else if(8 < args.Length() && args[8]->IsNumber() && args[9]->IsNumber()) {
		scale = vec3(args[8]->NumberValue(), args[8]->NumberValue(), args[8]->NumberValue());
		mass = (float)args[9]->NumberValue();
	}

	uint64_t id = module->CreateModel(mesh, material, position, radians(rotation), scale, mass);
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, to_string(id).c_str()));
}

void ModuleModel::jsLight(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 3 || !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first, second and third arguments must be position coordinates as numbers")));
		return;
	}
	vec3 position(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());

	if (args.Length() < 4 || !args[3]->IsNumber()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "fourth argument must be radis as number")));
		return;
	}
	float radius = (float)args[3]->NumberValue();

	vec3 color(1);
	if (6 < args.Length() && args[4]->IsNumber() && args[5]->IsNumber() && args[6]->IsNumber())
		color = vec3(args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue());

	float intensity = 1;
	if (7 < args.Length() && args[7]->IsNumber())
		intensity = (float)args[7]->NumberValue();

	Light::Shape shape = Light::POINT;
	if (8 < args.Length() && args[8]->IsString()) {
		string name = *String::Utf8Value(args[8]);
		if     (name == "point"      ) shape = Light::POINT;
		else if(name == "spot"       ) shape = Light::SPOT;
		else if(name == "directional") shape = Light::DIRECTIONAL;
		else isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "light type is unknown")));
	}

	uint64_t id = module->CreateLight(position, radius, color, intensity, shape);
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, to_string(id).c_str()));
}

void ModuleModel::jsPosition(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set position
	if (args.Length() > 1) {
		if (args.Length() < 4 || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be position coordinates as numbers")));
			return;
		}
		vec3 position(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		frm->Position(position);
	}
	// get position
	else {
		vec3 position = frm->Position();
		Handle<Array> result = Array::New(isolate, 3);
		result->Set(0, Number::New(isolate, position.x));
		result->Set(1, Number::New(isolate, position.y));
		result->Set(2, Number::New(isolate, position.z));
		args.GetReturnValue().Set(result);
	}
}

void ModuleModel::jsRotation(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 1 || !args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set rotation
	if (args.Length() > 1) {
		if (args.Length() < 4 || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be rotation angles in degrees as numbers")));
			return;
		}
		vec3 rotation(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		frm->Rotation(radians(rotation));
	}
	// get rotation
	else {
		vec3 rotation = degrees(frm->Rotation());
		Handle<Array> result = Array::New(isolate, 3);
		result->Set(0, Number::New(isolate, rotation.x));
		result->Set(1, Number::New(isolate, rotation.y));
		result->Set(2, Number::New(isolate, rotation.z));
		args.GetReturnValue().Set(result);
	}
}

void ModuleModel::jsScale(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set scale, buggy because changes all identical shapes
	if (args.Length() > 1)
	{
		vec3 scale;
		if (3 < args.Length()) {
			if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be scale values as numbers")));
				return;
			}
			scale = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		} else if (1 < args.Length()) {
			if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be scale values as number")));
				return;
			}
			scale = vec3(args[1]->NumberValue());
		}
		else {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second or second, third and fourth arguments must be scale values as numbers")));
			return;
		}

		frm->Body->activate(); // should be done by Scale() already but is needed for some reason
		frm->Scale(scale);
	}
	// get scale
	else {
		vec3 scale = frm->Scale();
		Handle<Array> result = Array::New(isolate, 3);
		result->Set(0, Number::New(isolate, scale.x));
		result->Set(1, Number::New(isolate, scale.y));
		result->Set(2, Number::New(isolate, scale.z));
		args.GetReturnValue().Set(result);
	}
}

void ModuleModel::jsDirection(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	vec3 direction = frm->Direction();

	Handle<Array> result = Array::New(isolate, 3);
	result->Set(0, Number::New(isolate, direction.x));
	result->Set(1, Number::New(isolate, direction.y));
	result->Set(2, Number::New(isolate, direction.z));
	args.GetReturnValue().Set(result);
}

void ModuleModel::jsRadius(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	// set radius
	if (args.Length() > 1) {
		if (!args[1]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a radius as number")));
			return;
		}
		lgt->Radius = (float)args[1]->NumberValue();
	}
	// get radius
	else {
		args.GetReturnValue().Set((double)lgt->Radius);
	}
}

void ModuleModel::jsIntensity(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	// set intensity
	if (args.Length() > 1) {
		if (!args[1]->IsNumber()) {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be an intensity as number")));
			return;
		}
		lgt->Intensity = (float)args[1]->NumberValue();
	}
	// get intensity
	else {
		args.GetReturnValue().Set((double)lgt->Intensity);
	}
}

void ModuleModel::jsColor(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if (!args[0]->IsString()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be entity id as string")));
		return;
	}
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	// set color
	if (args.Length() > 1) {
		vec3 color;
		if (args.Length() > 3) {
			if (!args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second, third and fourth arguments must be color values as numbers")));
				return;
			}
			color = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		}
		else if (args.Length() > 1) {
			if (!args[1]->IsNumber()) {
				isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second argument must be a color value as number")));
				return;
			}
			color = vec3(args[1]->NumberValue());
		}
		else {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "second or second, third and fourth arguments must be color values as numbers")));
			return;
		}
		lgt->Color = color;
	}
	// get color
	else {
		vec3 color = lgt->Color;
		Handle<Array> result = Array::New(isolate, 3);
		result->Set(0, Number::New(isolate, color.x));
		result->Set(1, Number::New(isolate, color.y));
		result->Set(2, Number::New(isolate, color.z));
		args.GetReturnValue().Set(result);
	}
}
