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
	texture = Texture();
	marker = Marker();

	running.store(true); loading.store(false);
	terrain = NULL, model = NULL, form = NULL;
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
	if(terrain && model && form && access.try_lock())
	{
		bool newone = !terrain->Changed;
		if(newone)
		{
			unsigned int id = Entity->New();
			Entity->Add<Terrain>(id, terrain);
			Entity->Add<Model>(id, model)->Diffuse = texture;
			Entity->Add<Form>(id, form)->Position(vec3(terrain->Chunk * CHUNK));
		}
		else
		{
			Debug->Pass("updated a chunk");
		}

		terrain = NULL, model = NULL, form = NULL;
		access.unlock();
	}

	/*
	// remesh changed chunks
	if(!terrain && !model && !form && access.try_lock())
	{
		for(auto i = tns.begin(); i != tns.end(); ++i)
		{
			if(i->second->Changed)
			{
				terrain = i->second;
				model = Entity->Get<Model>(i->first);
				form = Entity->Get<Form>(i->first); // make loader ready for that
				loading.store(true);
				break;
			}
		}
		access.unlock();
	}
	*/
	
	// mesh new in range chunks
	if(access.try_lock() && !terrain && !model && !form)
	{
		ivec3 i;
		bool found = false;
		for(i.x = -distance; i.x < distance && !found; ++i.x)
		for(i.z = -distance; i.z < distance && !found; ++i.z)
		{
			ivec3 key = i + camera;
			bool inrange = i.x * i.x + i.z * i.z < distance * distance;
			bool loaded = GetChunk(key) ? true : false;

			if(inrange && !loaded)
			{
				terrain = new Terrain();
				model = new Model();
				form = new Form();
				terrain->Chunk = key;

				found = true;
				loading.store(true);
			}
		}
		access.unlock();
	}

	// free out of range chunks
	int tolerance = 1;
	for(auto i : tns)
	{
		if(!Inside(abs(i.second->Chunk - camera), ivec3(0), ivec3(distance + tolerance)))
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
	if(sel.second)
		Entity->Get<Form>(marker)->Position(vec3(sel.first));
}

void ModuleTerrain::Listeners()
{

}
