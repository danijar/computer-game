#include "module.h"

#include <string>
#include <future>
#include <mutex>
#include <GLM/glm.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"
#include "form.h"
#include "model.h"
#include "settings.h"


void ModuleTerrain::Init()
{
	marker  = Marker();
	side    = Marker();
	texture = Texture();
	Entity->Get<Form>(marker)->Scale(vec3(1.1f));
	Entity->Get<Form>(side  )->Scale(vec3( .5f));

	type = 1;

	running = true, loading = false, null = true;
	task = async(launch::async, &ModuleTerrain::Loading, this);

	Listeners();
	Callbacks();
}

ModuleTerrain::~ModuleTerrain()
{
	glDeleteTextures(1, &texture);

	running.store(false);
	task.get();
}

void ModuleTerrain::Update()
{
	auto stg = Global->Get<Settings>("settings");
	auto tns = Entity->Get<Terrain>();
	ivec3 camera = ivec3(Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position() / vec3(CHUNK)); camera.y = 0;
	const int distance = (int)stg->Viewdistance / CHUNK / 10;

	// add loaded threads to entity system
	if(!loading && !null && access.try_lock())
	{
		if(current.Changed)
		{
			unsigned int id = GetChunk(current.Key);
			Entity->Get<Terrain>(id)->Changed = false;

			Buffer(id);
		}
		else
		{
			unsigned int id = Entity->New();
			Entity->Add<Terrain>(id, new Terrain(current));
			Entity->Add<Model>(id)->Diffuse = texture;
			Entity->Add<Form>(id)->Position(vec3(current.Key * CHUNK));

			Buffer(id);
		}
		access.unlock();
	}

	// remesh changed chunks
	if(!loading)
	{
		for(auto i = tns.begin(); i != tns.end(); ++i)
		{
			if(i->second->Changed)
			{
				current = Terrain(*i->second);
				null = false;
				loading = true;
				break;
			}
		}
	}
	
	// mesh new in range chunks
	if(!loading && access.try_lock())
	{
		ivec3 i;
		bool loop = true;
		for(i.x = -distance; i.x < distance && loop; ++i.x)
		for(i.z = -distance; i.z < distance && loop; ++i.z)
		{
			ivec3 key = i + camera;
			bool inrange = i.x * i.x + i.z * i.z < distance * distance;
			bool loaded = GetChunk(key) ? true : false;

			if(inrange && !loaded)
			{
				current = Terrain();
				current.Key = key;
				null = false;
				loading = true;

				loop = false;
			}
		}
		access.unlock();
	}

	// free out of range chunks
	int tolerance = 1;
	for(auto i : tns)
	{
		if(!Inside(abs(i.second->Key - camera), ivec3(0), ivec3(distance + tolerance)))
		{
			auto mdl = Entity->Get<Model>(i.first);
			glDeleteBuffers(1, &mdl->Positions);
			glDeleteBuffers(1, &mdl->Normals);
			glDeleteBuffers(1, &mdl->Texcoords);
			glDeleteBuffers(1, &mdl->Elements);

			Entity->Delete(i.first);
		}
	}

	// selection
	auto sel = Selection();
	if(get<2>(sel))
	{
		Entity->Get<Form>(marker)->Position(vec3(get<0>(sel))                           + vec3((1.0f - 1.1f) / 2));
		Entity->Get<Form>(side  )->Position(vec3(get<0>(sel)) + .5f * vec3(get<1>(sel)) + vec3((1.0f -  .5f) / 2));
	}
	else
	{
		Entity->Get<Form>(marker)->Position(vec3(0, -1000, 0));
		Entity->Get<Form>(side  )->Position(vec3(0, -1000, 0));
	}
}

void ModuleTerrain::Listeners()
{
	Event->Listen("InputBindMine", [=]{
		auto sel = Selection();
		SetBlock(get<0>(sel), 0);
	});

	Event->Listen("InputBindPlace", [=]{
		auto sel = Selection();
		SetBlock(get<0>(sel) + get<1>(sel), type);
	});
}
