#include "module.h"

#include <string>
#include <future>
#include <GLM/glm.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"
#include "form.h"
#include "model.h"


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
	/*
	auto tns = Entity->Get<Terrain>();
	ivec3 camera = (ivec3)Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position();
	*/

	// add loaded threads to entity system
	if(!loading)
		if(terrain != NULL && model != NULL)
		{
			if(!terrain->Changed)
			{
				model->Diffuse = texture;

				unsigned int id = Entity->New();
				Entity->Add<Terrain>(id, terrain);
				Entity->Add<Model>(id, model);
				Entity->Add<Form>(id);

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

	// mesh new in range chunks
	if(!loading)
		if(terrain == NULL && model == NULL)
		{
			// ...
		}

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
