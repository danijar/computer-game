#include "module.h"

#include <string>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "type/terrain/type.h"
#include "type/model/type.h"
#include "type/form/type.h"

using namespace std;
using namespace glm;
using namespace v8;

void ModuleTerrain::Callbacks()
{
	Script->Bind("chunk",       jsChunk      );
	Script->Bind("block",       jsBlock      );
	Script->Bind("placetype",   jsPlacetype  );
	Script->Bind("placemarker", jsPlacemarker);
}

void ModuleTerrain::jsChunk(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if (!module->loading && module->access.try_lock()) {
		if (2 < args.Length() && args[0]->IsInt32() && args[1]->IsInt32() && args[2]->IsInt32()) {
			ivec3 key(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());
			module->current = Terrain();
			module->current.Key = key;
			module->null = false;
			module->loading = true;
			ManagerLog::Print("script", "load new chunk at (" + to_string(key.x) + ", " + to_string(key.y) + ", " + to_string(key.z) + ")");
		}
		module->access.unlock();
	} else {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "another chunk is loaded at the moment")));
	}
}

void ModuleTerrain::jsBlock(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if (args.Length() < 3 || !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32()) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first, second and third arguments must be block coordinates as numbers")));
		return;
	}
	ivec3 block(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());

	// containing chunk is not loaded
	if (!module->GetChunk(module->PosChunk(block))) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "the chunk is not loaded")));
		return;
	}

	// set block
	if (3 < args.Length() && args[3]->IsUint32()) {
		uint8_t type = args[3]->Uint32Value();
		module->SetBlock(block, type);
	}
	// get block
	else {
		uint8_t type = module->GetBlock(block);
		args.GetReturnValue().Set(type);
	}
}

void ModuleTerrain::jsPlacetype(const FunctionCallbackInfo<Value> &args)
{
	Isolate* isolate = args.GetIsolate();
	HandleScope scope(isolate);
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	// set place type
	if (0 < args.Length()) {
		if (args[0]->IsUint32()) {
			module->type = args[0]->Uint32Value();
		} else if (args[0]->IsString()) {
			string name = *String::Utf8Value(args[0]);
			vector<string> types;
			types.push_back("air");
			types.push_back("dirt");
			types.push_back("grass");
			types.push_back("rock");
			types.push_back("sand");
			types.push_back("wood");
			types.push_back("planks");
			types.push_back("leaves");
			types.push_back("coal");
			types.push_back("cobble");
			types.push_back("concrete");
			auto i = find(types.begin(), types.end(), name);
			if (i != types.end())
				module->type = i - types.begin();
		}
		else  {
			isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "first argument must be block type as number or string")));
			return;
		}
	}
	// get place type
	else {
		args.GetReturnValue().Set(module->type);
	}
}

void ModuleTerrain::jsPlacemarker(const FunctionCallbackInfo<Value> &args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());
	module->show = !module->show;
	if(!module->show)
		module->Entity->Get<Form>(module->marker)->Position(vec3(0, -9999, 0));
	ManagerLog::Print("script", string(module->show ? "enabled" : "disabled") + " placing marker");
}
