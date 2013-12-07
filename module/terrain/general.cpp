#include "module.h"

#include <string>
#include <future>
#include <mutex>
#include "dependency/glm/glm.hpp"
#include "dependency/sfml/Window.hpp"
using namespace std;
using namespace glm;
using namespace sf;

#include "type/terrain/type.h"
#include "type/form/type.h"
#include "type/model/type.h"
#include "type/settings/type.h"
#include "type/camera/type.h"
#include "type/person/type.h"
#include "type/print/type.h"


void ModuleTerrain::Init()
{
	texture = Texture();

	marker = Marker();
	Entity->Get<Form>(marker)->Scale(vec3(.5f));

	show = true; type = 1;

	running = true, loading = false, null = true;
	task = async(launch::async, &ModuleTerrain::Loading, this);

	Entity->Add<Print>(Entity->New())->Text = [=]{
		ivec3 position = get<0>(Selection());
		return "selection X " + to_string(position.x) + " Y " + to_string(position.y) + " Z " + to_string(position.z);
	};

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
	ivec3 camera = ivec3(Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Position()) / CHUNK_SIZE;
	/*
	 * load chunks independent from cameras height
	 * camera.y = 0;
	 */
	ivec3 distance = (int)(*stg->Get<float>("Viewdistance") * *stg->Get<float>("Chunkdistance")) / CHUNK_SIZE;
	/*
	 * this adds a lot more load to the CPU
	 * distance.y = std::max(distance.y / 2, 1);
	 */
	distance.y = 1;

	// add loaded chunks to entity system
	if(!loading && !null && access.try_lock())
	{
		if(current.Changed)
		{
			uint64_t id = GetChunk(current.Key);
			Entity->Get<Terrain>(id)->Changed = false;

			Buffer(id);
		}
		else
		{
			uint64_t id = Entity->New();
			Entity->Add<Terrain>(id, new Terrain(current));
			Entity->Add<Model>(id)->Diffuse = texture;
			Entity->Add<Form>(id)->Position(vec3(current.Key * CHUNK_SIZE));

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
		bool found = false;
		ivec3 i, nearest;
		for(i.x = -distance.x; i.x < distance.x; ++i.x)
		for(i.y = -distance.y; i.y < distance.y; ++i.y)
		for(i.z = -distance.z; i.z < distance.z; ++i.z)
		{
			bool inrange = float(i.x*i.x) / float(distance.x*distance.x) + float(i.z*i.z) / float(distance.z*distance.z) < 1.0f;
			bool nearer = found ? length(vec3(i)) < length(vec3(nearest)) : true;
			bool loaded = GetChunk(camera + i) ? true : false;
			if(inrange && nearer && !loaded)
			{
				nearest = camera + i;
				found = true;
			}
		}
		if(found)
		{
			current = Terrain();
			current.Key = nearest;
			null = false;
			loading = true;
		}
		else
		{
			static bool once = true;
			if(once)
			{
				Event->Fire("TerrainLoadingFinished");
				once = false;
			}
		}
		access.unlock();
	}

	// free out of range chunks
	int tolerance = 1;
	for(auto i : tns)
	{
		if(!Inside(abs(i.second->Key - camera), ivec3(0), distance + ivec3(tolerance)))
		{
			auto mdl = Entity->Get<Model>(i.first);
			if(mdl->Elements)  glDeleteBuffers(1, &mdl->Positions);
			if(mdl->Normals)   glDeleteBuffers(1, &mdl->Normals);
			if(mdl->Texcoords) glDeleteBuffers(1, &mdl->Texcoords);
			if(mdl->Elements)  glDeleteBuffers(1, &mdl->Elements);

			Entity->Delete(i.first);
		}
	}

	// selection
	if(show)
	{
		auto sel = Selection();
		if(get<2>(sel) && InReachDistance((vec3)get<0>(sel) + vec3(0.5)))
			Entity->Get<Form>(marker)->Position(vec3(get<0>(sel)) + .3f * vec3(get<1>(sel)) + vec3((1.0f -  .5f) / 2));
		else
			Entity->Get<Form>(marker)->Position(vec3(0, -9999, 0));
	}
}

void ModuleTerrain::Listeners()
{
	Event->Listen("SaveChanged", [=]{
		while(loading);
		null = true;

		auto tns = Entity->Get<Terrain>();

		for(auto i : tns)
		{
			auto mdl = Entity->Get<Model>(i.first);
			glDeleteBuffers(1, &mdl->Positions);
			glDeleteBuffers(1, &mdl->Normals);
			glDeleteBuffers(1, &mdl->Texcoords);
			glDeleteBuffers(1, &mdl->Elements);

			Entity->Delete(i.first);
		}
	});

	Event->Listen("InputBindMine", [=]{
		auto sel = Selection();
		if(InReachDistance((vec3)get<0>(sel) + vec3(0.5)))
			SetBlock(get<0>(sel), 0);
	});

	Event->Listen("InputBindPlace", [=]{
		auto sel = Selection();
		if(get<2>(sel) && InReachDistance((vec3)get<0>(sel) + vec3(0.5)))
		{
			uint64_t person = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->Person;
			if(!person) return;
			auto psn = Entity->Get<Person>(person);

			vec3 player = Entity->Get<Form>(person)->Position();
			ivec3 block = get<0>(sel) + get<1>(sel);

			if(player.x + psn->Radius < block.x || player.x - psn->Radius > block.x + 1 ||
			   player.y + psn->Height < block.y || player.y               > block.y + 1 ||
			   player.z + psn->Radius < block.z || player.z - psn->Radius > block.z + 1)
					SetBlock(block, type);
			else Log->Warning("could not place block");
		}
	});

	Event->Listen("InputBindPick", [=]{
		auto sel = Selection();
		type = get<2>(sel);
	});

	Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
		bool changed = true;

		switch(Code)
		{
		case Keyboard::Num1:
			type = 1;
			break;
		case Keyboard::Num2:
			type = 2;
			break;
		case Keyboard::Num3:
			type = 3;
			break;
		case Keyboard::Num4:
			type = 4;
			break;
		case Keyboard::Num5:
			type = 5;
			break;
		case Keyboard::Num6:
			type = 6;
			break;
		case Keyboard::Num7:
			type = 7;
			break;
		case Keyboard::Num8:
			type = 8;
			break;
		case Keyboard::Num9:
			type = 9;
			break;
		default:
			changed = false;
		}

		if(changed)
			Log->Print("changed placing type to " + to_string(type));
	});
}
