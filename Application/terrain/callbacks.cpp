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

	if(module->loading || module->terrain != NULL || module->model != NULL)
	{
		HelperDebug::Fail("script", "another chunk is loaded or added at the moment");
		return v8::Undefined();
	}

	if(2 < args.Length() && args[0]->IsInt32() && args[1]->IsInt32() && args[2]->IsInt32())
	{
		auto terrain = new Terrain();
		Model *model = new Model();
		terrain->Chunk = ivec3(args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value());

		module->terrain = terrain;
		module->model = model;

		module->loading = true;
	}

	return v8::Undefined();
}
