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
	ivec3 camera = ivec3(Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position() / vec3(CHUNK));

	// add loaded threads to entity system
	if(!loading && terrain && model)
	{
		bool newone = !terrain->Changed;

		if(newone)
		{
			model->Diffuse = texture;

			unsigned int id = Entity->New();
			Entity->Add<Terrain>(id, terrain);
			Entity->Add<Model>(id, model);
			Entity->Add<Form>(id)->Position(vec3(terrain->Chunk * CHUNK));

			Debug->Pass("added a chunk to entity system");
		}
		else
		{
			Debug->Pass("updated a chunk");
		}

		terrain = NULL, model = NULL;
	}

	/*
	// remesh changed chunks
	if(!loading)
		if(terrain == NULL && model == NULL)
			for(auto i = tns.begin(); i != tns.end(); ++i)
				if(i->second->Changed)
				{
					terrain = i->second;
					model = Entity->Get<Model>(i->first);
					loading = true;
					break;
				}

	*/

	// mesh new in range chunks
	const int distance = (int)stg->Viewdistance / CHUNK / 7;
	if(!loading && !terrain && !model)
	{
		ivec3 i;
		for(i.x = -distance; i.x < distance && !loading; ++i.x)
		for(i.z = -distance; i.z < distance && !loading; ++i.z)
		{
			ivec3 key = i + ivec3(camera.x, 0, camera.z);
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

	/*
	// free out of range chunks
	ivec3 distance = ivec3(1000) / CHUNK;
	for(auto i : tns)
		if(!Inside(i.second->Chunk - camera, -distance, distance))
		{
			auto mdl = Entity->Get<Model>(i.first);
			glDeleteBuffers(1, &mdl->Positions);
			glDeleteBuffers(1, &mdl->Normals);
			glDeleteBuffers(1, &mdl->Texcoords);
			glDeleteBuffers(1, &mdl->Elements);
			Entity->Delete(i.first);

			Debug->Print("chunk (" + to_string(i.second->Chunk.x) + ", " + to_string(i.second->Chunk.y) + ", " + to_string(i.second->Chunk.z) + ") out of range");
		}
	*/
}

void ModuleTerrain::Listeners()
{

}
