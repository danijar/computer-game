#include "module.h"

#include <string>
#include <future>
#include <mutex>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <sfml/Window.hpp>

#include "type/terrain/type.h"
#include "type/form/type.h"
#include "type/model/type.h"
#include "type/settings/type.h"
#include "type/camera/type.h"
#include "type/person/type.h"
#include "type/print/type.h"

using namespace std;
using namespace glm;
using namespace sf;

void ModuleTerrain::Init()
{
	diffuse  = Texture("terrain_d.png");
	normal   = Texture("terrain_n.jpg");
	specular = Texture("terrain_s.jpg");

	marker = Marker();
	Entity->Get<Form>(marker)->Scale(vec3(.5f));

	show = true, type = 1;

	running = true, loading = false, null = true;
	task = async(launch::async, &ModuleTerrain::Loading, this);

	candidates = 0;

	Entity->Add<Print>(Entity->New())->Text = [=]{
		ivec3 position = get<0>(Selection());
		return "selection X " + to_string(position.x) + " Y " + to_string(position.y) + " Z " + to_string(position.z);
	};

	Entity->Add<Print>(Entity->New())->Text = [=]{
		return "pending chunks " + to_string(candidates);
	};

	Listeners();
	Callbacks();
}

ModuleTerrain::~ModuleTerrain()
{
	glDeleteTextures(1, &diffuse);
	glDeleteTextures(1, &normal);
	glDeleteTextures(1, &specular);

	running = false;
	task.get();
}

void ModuleTerrain::Update()
{
	auto stg = Global->Get<Settings>("settings");
	auto tns = Entity->Get<Terrain>();

	// ignore camera height to get an overview flying above the chunks
	const bool grounding = false;

	vec3 camera = Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Position();
	if (grounding) camera.y = 0;

	float inradius = (*stg->Get<float>("Viewdistance")) * (*stg->Get<float>("Chunkdistance"));
	float outradius = inradius + 0.3f * length(vec3(CHUNK_SIZE));

	// add loaded chunks to entity system
	if(!loading && !null && access.try_lock())
	{
		// chunk was just modified
		if(current.Changed) {
			uint64_t id = GetChunk(current.Key);
			Entity->Get<Terrain>(id)->Changed = false;
			Buffer(id);
		}
		// chunk was newly loaded
		else {
			uint64_t id = Entity->New();
			Entity->Add<Terrain>(id, new Terrain(current));
			auto mdl = Entity->Add<Model>(id);
			auto frm = Entity->Add<Form>(id);
			// material
			mdl->Diffuse   = diffuse;
			mdl->Normal    = normal;
			mdl->Specular  = specular;
			mdl->Shininess = 1.0f;
			// bounding box, later move into form type
			mdl->Box.X.Max = (float)CHUNK_SIZE.x;
			mdl->Box.Y.Max = (float)CHUNK_SIZE.y;
			mdl->Box.Z.Max = (float)CHUNK_SIZE.z;
			// position
			frm->Position(vec3(current.Key * CHUNK_SIZE));
			// vertices
			Buffer(id);
		}
		access.unlock();
	}

	// remesh changed chunks
	if(!loading) {
		for(auto i = tns.begin(); i != tns.end(); ++i) {
			if(i->second->Changed) {
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
		// relative range to check for chunk candidates
		ivec3 range(ivec3(inradius) / CHUNK_SIZE + ivec3(1));
		range.y = 1;

		// offset of relative range to camera
		ivec3 offset = ivec3(Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Position() + vec3(0.5f)) / CHUNK_SIZE;
		if (grounding) offset.y = 0;

		// find nearest chunk candidate
		ivec3 i, nearest;
		float best = -1;
		candidates = 0;
		for (i.x = -range.x; i.x <= range.x; ++i.x)
		for (i.y = -range.y; i.y <= range.y; ++i.y)
		for (i.z = -range.z; i.z <= range.z; ++i.z) {
			// calculate distance from chunk center to camera
			vec3 center = (vec3(offset + i) + vec3(0.5f)) * vec3(CHUNK_SIZE);
			float distance = length(camera - center);

			// compute criteria
			bool inrange = distance < inradius;
			bool unloaded = !GetChunk(offset + i);
			bool nearer = best < 0 ? true : distance < best;

			// consider if in range and not loaded
			if (inrange && unloaded) {
				candidates++;
				// find nearest
				if (nearer) {
					nearest = offset + i;
					best = distance;
				}
			}
		}

		// load chunk if one was found
		if(best > -1) {
			current = Terrain();
			current.Key = nearest;
			null = false;
			loading = true;
		} else {
			static bool once = true;
			if(once) {
				Event->Fire("TerrainLoadingFinished");
				once = false;
			}
		}
		access.unlock();
	}

	// free out of range chunks
	for(auto i : tns) {
		auto chunk = i.second;
		// calculate distance from chunk center to camera
		vec3 center = (vec3(chunk->Key) + vec3(0.5f)) * vec3(CHUNK_SIZE);
		float distance = length(camera - center);

		// free if out of range
		if(distance > outradius) {
			auto mdl = Entity->Get<Model>(i.first);
			if(mdl->Elements)  glDeleteBuffers(1, &mdl->Positions);
			if(mdl->Normals)   glDeleteBuffers(1, &mdl->Normals);
			if(mdl->Texcoords) glDeleteBuffers(1, &mdl->Texcoords);
			if(mdl->Elements)  glDeleteBuffers(1, &mdl->Elements);

			Entity->Delete(i.first);
		}
	}

	// selection
	if(show) {
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
