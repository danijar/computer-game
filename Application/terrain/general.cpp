#include "module.h"

#include <string>
#include <future>
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

	loading = false, running = true;
	terrain = NULL, model = NULL;
	task = async(launch::async, &ModuleTerrain::Loading, this);

	Listeners();
	Callbacks();
}

ModuleTerrain::~ModuleTerrain()
{
	glDeleteTextures(1, &texture);

	running = false;
	task.get();
}

void ModuleTerrain::Update()
{
	auto stg = Global->Get<Settings>("settings");
	auto tns = Entity->Get<Terrain>();
	ivec3 camera = ivec3(Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position() / vec3(CHUNK)); camera.y = 0;

	// add loaded threads to entity system
	if(!loading && terrain && model)
	{
		bool newone = !terrain->Changed;

		if(newone)
		{
			unsigned int id = Entity->New();
			Entity->Add<Terrain>(id, terrain);
			Entity->Add<Model>(id, model)->Diffuse = texture;
			Entity->Add<Form>(id)->Position(vec3(terrain->Chunk * CHUNK));
		}
		else
		{
			Debug->Pass("updated a chunk");
		}

		terrain = NULL, model = NULL;
	}

	/*
	// remesh changed chunks
	if(!loading && !terrain && !model)
	{
		for(auto i = tns.begin(); i != tns.end() && !loading; ++i)
		{
			if(i->second->Changed)
			{
				terrain = i->second;
				model = Entity->Get<Model>(i->first);
				loading = true;
			}
		}
	}
	*/

	// mesh new in range chunks
	const int distance = (int)stg->Viewdistance / CHUNK / 10;
	if(!loading && !terrain && !model)
	{
		ivec3 i;
		for(i.x = -distance; i.x < distance && !loading; ++i.x)
		for(i.z = -distance; i.z < distance && !loading; ++i.z)
		{
			ivec3 key = i + camera;
			bool inrange = i.x * i.x + i.z * i.z < distance * distance;
			bool loaded = GetChunk(key) ? true : false;

			if(inrange && !loaded)
			{
				terrain = new Terrain();
				model = new Model();
				terrain->Chunk = key;

				loading = true;
			}
		}
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
}

void ModuleTerrain::Listeners()
{

}
