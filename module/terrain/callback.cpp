#include "module.h"

#include <string>
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
using namespace v8;

#include "type/terrain/type.h"
#include "type/model/type.h"
#include "type/form/type.h"


void ModuleTerrain::Callbacks()
{
	Script->Bind("chunk",       jsChunk      );
	Script->Bind("block",       jsBlock      );
	Script->Bind("placetype",   jsPlacetype  );
	Script->Bind("placemarker", jsPlacemarker);
}

Handle<Value> ModuleTerrain::jsChunk(const Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if(!module->loading && module->access.try_lock())
	{
		if(2 < args.Length() && args[0]->IsInt32() && args[1]->IsInt32() && args[2]->IsInt32())
		{
			ivec3 key(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());

			module->current = Terrain();
			module->current.Key = key;
			module->null = false;
			module->loading = true;
		}
		module->access.unlock();
	}
	else
	{
		ManagerLog::Fail("script", "another chunk is loaded or added at the moment");
	}

	return Undefined();
}

Handle<Value> ModuleTerrain::jsBlock(const Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 3 || !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32())
		return Undefined();
	ivec3 block(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());

	// containing chunk is not loaded
	if(!module->GetChunk(module->PosChunk(block)))
		return Undefined();

	// set block
	if(3 < args.Length() && args[3]->IsUint32())
	{
		uint8_t type = args[3]->Uint32Value();
		module->SetBlock(block, type);
		return Undefined();
	}
	// get block
	else
	{
		uint8_t type = module->GetBlock(block);
		return Uint32::New(type);
	}
}

Handle<Value> ModuleTerrain::jsPlacetype(const Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	// set place type
	if(0 < args.Length())
	{
		if(args[0]->IsUint32())
		{
			module->type = args[0]->Uint32Value();
		}
		else if(args[0]->IsString())
		{
			string name = *String::Utf8Value(args[0]);
			if(name == "air")
				module->type = 0;
			else if(name == "dirt")
				module->type = 1;
			else if(name == "grass")
				module->type = 2;
		}
		return Undefined();
	}
	// get place type
	else
	{
		return Uint32::New(module->type);
	}
}

Handle<Value> ModuleTerrain::jsPlacemarker(const Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	module->show = !module->show;

	if(!module->show)
		module->Entity->Get<Form>(module->marker)->Position(vec3(0, -9999, 0));

	ManagerLog::Print("script", string(module->show ? "enabled" : "disabled") + " placing marker");
	return Undefined();
}
