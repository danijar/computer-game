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

	loading = false;
	cancle = false;

	task = async(launch::async, &ModuleTerrain::Loading, this);

	Listeners();
	Callbacks();
}

ModuleTerrain::~ModuleTerrain()
{
	glDeleteTextures(1, &texture);

	cancle = true;
	task.get();
}

void ModuleTerrain::Update()
{
	auto tns = Entity->Get<Terrain>();
	ivec3 camera = (ivec3)Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position();

	// remesh changed chunks
	if(!loading)
		for(auto i : tns)
			if(i.second->Changed)
			{
				loading_chunk = i.second->Chunk;
				loading = true;
			}

	// mesh new in range chunks
	if(!loading)
		;

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
}

void ModuleTerrain::Listeners()
{

}
