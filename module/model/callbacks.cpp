#include "module.h"

#include <string>
#include <dependency/glm/glm.hpp>
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

Handle<Value> ModuleModel::jsModel(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString())
		return Undefined();
	string mesh = *String::Utf8Value(args[0]);
	string material = *String::Utf8Value(args[1]);

	if(args.Length() < 5 || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsNumber())
		return Undefined();
	vec3 position(args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue());

	vec3 rotation; // rotation is passed in degrees from script
	if(7 < args.Length() && args[5]->IsNumber() && args[6]->IsNumber() && args[7]->IsNumber())
		rotation = vec3(args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue());

	vec3 scale(1);
	float mass = 0;
	if(10 < args.Length() && args[8]->IsNumber() && args[9]->IsNumber() && args[10]->IsNumber() && args[11]->IsNumber())
	{
		scale = vec3(args[8]->NumberValue(), args[9]->NumberValue(), args[10]->NumberValue());
		mass = (float)args[11]->NumberValue();
	}
	else if(8 < args.Length() && args[8]->IsNumber() && args[9]->IsNumber())
	{
		scale = vec3(args[8]->NumberValue(), args[8]->NumberValue(), args[8]->NumberValue());
		mass = (float)args[9]->NumberValue();
	}

	uint64_t id = module->CreateModel(mesh, material, position, radians(rotation), scale, mass);
	return String::New(to_string(id).c_str());
}

Handle<Value> ModuleModel::jsLight(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 3 || !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
		return Undefined();
	vec3 position(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());

	if(args.Length() < 4 || !args[3]->IsNumber())
		return Undefined();
	float radius = (float)args[3]->NumberValue();

	vec3 color(1);
	if(6 < args.Length() && args[4]->IsNumber() && args[5]->IsNumber() && args[6]->IsNumber())
		color = vec3(args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue());

	float intensity = 1;
	if(7 < args.Length() && args[7]->IsNumber())
		intensity = (float)args[7]->NumberValue();

	Light::Shape shape = Light::POINT;
	if(8 < args.Length() && args[8]->IsString())
	{
		string name = *String::Utf8Value(args[8]);
		if     (name == "point"      ) shape = Light::POINT;
		else if(name == "spot"       ) shape = Light::SPOT;
		else if(name == "directional") shape = Light::DIRECTIONAL;
		else ManagerLog::Fail("script", "light unknown type");
	}

	uint64_t id = module->CreateLight(position, radius, color, intensity, shape);
	return String::New(to_string(id).c_str());
}

Handle<Value> ModuleModel::jsPosition(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set position
	if(args.Length() > 1)
	{
		vec3 position;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			position = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		// else if position passed as array
			// position = ...
		else
			return Undefined();

		frm->Position(position);
		return Undefined();
	}
	// get position
	else
	{
		vec3 position = frm->Position();

		Handle<Array> result = Array::New(3);
		result->Set(0, Number::New(position.x));
		result->Set(1, Number::New(position.y));
		result->Set(2, Number::New(position.z));
		return result;
	}
}

Handle<Value> ModuleModel::jsRotation(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set rotation
	if(args.Length() > 1)
	{
		vec3 rotation;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			rotation = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		// else if rotation passed as array
			// rotation = ...
		else
			return Undefined();

		frm->Rotation(radians(rotation));
		return Undefined();
	}
	// get rotation
	else
	{
		vec3 rotation = degrees(frm->Rotation());

		Handle<Array> result = Array::New(3);
		result->Set(0, Number::New(rotation.x));
		result->Set(1, Number::New(rotation.y));
		result->Set(2, Number::New(rotation.z));
		return result;
	}
}

Handle<Value> ModuleModel::jsScale(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	// set scale, buggy because changes all identical shapes
	if(args.Length() > 1)
	{
		vec3 scale;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			scale = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		// else if scale passed as array
			// scale = ...
		else if(1 < args.Length() && args[1]->IsNumber())
			scale = vec3(args[1]->NumberValue());
		else
			return Undefined();

		frm->Body->activate(); // is done by Scale() already but is needed for some reason
		frm->Scale(scale);
		return Undefined();
	}
	// get scale
	else
	{
		vec3 scale = frm->Scale();

		Handle<Array> result = Array::New(3);
		result->Set(0, Number::New(scale.x));
		result->Set(1, Number::New(scale.y));
		result->Set(2, Number::New(scale.z));
		return result;
	}
}

Handle<Value> ModuleModel::jsDirection(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto frm = module->Entity->Get<Form>(id);

	vec3 direction = frm->Direction();

	Handle<Array> result = Array::New(3);
	result->Set(0, Number::New(direction.x));
	result->Set(1, Number::New(direction.y));
	result->Set(2, Number::New(direction.z));
	return result;
}

Handle<Value> ModuleModel::jsRadius(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	if(args.Length() > 1)
	{
		if(args[1]->IsNumber())
			lgt->Radius = (float)args[1]->NumberValue();
		return Undefined();
	}
	else
	{
		return Number::New(lgt->Radius);
	}
}

Handle<Value> ModuleModel::jsIntensity(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	if(args.Length() > 1)
	{
		if(args[1]->IsNumber())
			lgt->Intensity = (float)args[1]->NumberValue();
		return Undefined();
	}
	else
	{
		return Number::New(lgt->Intensity);
	}
}

Handle<Value> ModuleModel::jsColor(const Arguments& args)
{
	ModuleModel *module = (ModuleModel*)HelperScript::Unwrap(args.Data());

	if(!args[0]->IsString())
		return Undefined();
	uint64_t id = std::stoull(*String::Utf8Value(args[0]));
	auto lgt = module->Entity->Get<Light>(id);

	// set color
	if(args.Length() > 1)
	{
		vec3 color;
		if(3 < args.Length() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber())
			color = vec3(args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
		// else if color passed as array
			// color = ...
		else if(1 < args.Length() && args[1]->IsNumber())
			color = vec3(args[1]->NumberValue());
		else
			return Undefined();

		lgt->Color = color;
		return Undefined();
	}
	// get color
	else
	{
		vec3 color = lgt->Color;

		Handle<Array> result = Array::New(3);
		result->Set(0, Number::New(color.x));
		result->Set(1, Number::New(color.y));
		result->Set(2, Number::New(color.z));
		return result;
	}
}
