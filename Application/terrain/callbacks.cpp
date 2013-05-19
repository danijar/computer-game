#include "module.h"

#include <GLM/glm.hpp>
using namespace glm;

#include "terrain.h"
#include "model.h"


void ModuleTerrain::Callbacks()
{
	Script->Bind("chunk", jsChunk);
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
			module->current.Chunk = key;
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
