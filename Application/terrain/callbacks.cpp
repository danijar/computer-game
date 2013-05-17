#include "module.h"

#include <GLM/glm.hpp>
using namespace glm;


void ModuleTerrain::Callbacks()
{
	Script->Bind("chunk", jsChunk);
}

v8::Handle<v8::Value> ModuleTerrain::jsChunk(const v8::Arguments& args)
{
	ModuleTerrain *module = (ModuleTerrain*)HelperScript::Unwrap(args.Data());

	if(module->loading)
	{
		HelperDebug::Fail("script", "another chunk is loaded at the moment");
		return v8::Undefined();
	}

	if(2 < args.Length() && args[0]->IsInt32() && args[1]->IsInt32() && args[2]->IsInt32())
	{
		module->loading_chunk = ivec3(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());
		module->loading = true;
	}

	return v8::Undefined();
}
