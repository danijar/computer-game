#include "module.h"

#include <GLM/glm.hpp>
using namespace glm;

#include "terrain.h"
#include "model.h"


void ModuleTerrain::Callbacks()
{
	Script->Bind("chunk", jsChunk);
	Script->Bind("block", jsBlock);
}

v8::Handle<v8::Value> ModuleTerrain::jsChunk(const v8::Arguments& args)
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
		HelperDebug::Fail("script", "another chunk is loaded or added at the moment");
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleTerrain::jsBlock(const v8::Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 3 || !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsInt32())
		return v8::Undefined();
	ivec3 block(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());

	if(!module->GetChunk(module->PosChunk(block)))
	{
		HelperDebug::Fail("script", "the chunk containing that block is not loaded");
		return v8::Undefined();
	}

	// set block
	if(3 < args.Length() && args[3]->IsUint32())
	{
		uint8_t type = args[3]->Uint32Value();
		module->SetBlock(block, type);
		return v8::Undefined();
	}
	// get block
	else
	{
		uint8_t type = module->GetBlock(block);
		return v8::Uint32::New(type);
	}
}
