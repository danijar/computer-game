#include "module.h"

#include <string>
#include <bullet/btBulletDynamicsCommon.h>
#include "dependency/glm/glm.hpp"
#include "dependency/sfml/Window.hpp"

#include <thread>
#include <chrono>

#include "type/form/type.h"
#include "type/person/type.h"
//#include "type/person/trait.h"
#include "type/camera/type.h"
#include "type/print/type.h"

using namespace std;
using namespace glm;
using namespace sf;


void ModulePerson::Init()
{
	Listeners();
	Callbacks();

	/*
	// debug
	Person *psn = new Person;
	psn->Height = 1.77f;
	psn->Mass = 67.f;
	Data->Save(Entity->New(), psn);
	*/

	Script->Run("init.js");
}

void ModulePerson::Update()
{
	Script->Run("update.js");

	// update persons
	auto pns = Entity->Get<Person>();
	
	for(auto i = pns.begin(); i != pns.end(); ++i)
	{
		auto psn = i->second;
		if(psn->Changed)
		{
			Setup(i->first);
			psn->Changed = false;
		}

		Ground(i->first);
	}

	// move person attached to active camera
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));
	uint64_t id = cam->Person;

	if(id)
	{
		auto psn = Entity->Get<Person>(id);
		auto frm = Entity->Get<Form>(id);

		vec3 move;
		if(cam->Active)
		{
			if (Keyboard::isKeyPressed(Keyboard::Up   ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
			if (Keyboard::isKeyPressed(Keyboard::Down ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
			if (Keyboard::isKeyPressed(Keyboard::Left ) || Keyboard::isKeyPressed(Keyboard::A)) move.z++;
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) move.z--;
		}
		if(length(move))
		{
			if(Keyboard::isKeyPressed(Keyboard::LShift))
				Move(id, move, 20.0f);
			else if(Ground(id))
				Move(id, move);
			else if(Edge(id, move))
				Move(id, move);

			psn->Walking = true;
		}
		else if(psn->Walking)
		{
			if(Ground(id))
				frm->Body->setLinearVelocity(btVector3(0, 0, 0));

			psn->Walking = false;
		}

		if(psn->Jumping && !Keyboard::isKeyPressed(Keyboard::Space) && Ground(id))
			psn->Jumping = false;
	}

	// store player position every some milliseconds
	static Clock clock;
	if(clock.getElapsedTime().asMilliseconds() > 500)
	{
		Save();
		clock.restart();
	}
}

void ModulePerson::Listeners()
{
	Event->Listen("InputBindJump", [=]{
		uint64_t id = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->Person;
		if(!id) return;
		auto psn = Entity->Get<Person>(id);
		auto frm = Entity->Get<Form>(id);

		if(Keyboard::isKeyPressed(Keyboard::LShift))
			Jump(id, 15.0f, true);
		else if(Ground(id))
			Jump(id);
	});

	Event->Listen("TerrainLoadingFinished", [=]{
		// load persons
		Load();

		// if there is no person at all, add a default one at camera position
		if(Entity->Get<Person>().size() < 1)
		{
			uint64_t id = Entity->New();
			auto psn = Entity->Add<Person>(id);
			auto frm = Entity->Add<Form>(id);
			
			vec3 position = Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Position() - vec3(0, psn->Eyes, 0);
			psn->Calculate(1.80f);
			frm->Position(position);

			Log->Pass("none loaded, added default one");
		}

		// if there is no person bound to camera, bound the first one
		auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));
		if(!cam->Person)
		{
			uint64_t id = Entity->Get<Person>().begin()->first;
			cam->Person = id;
			Log->Pass("not bound to camera, fallback to first one");
		}

		// add player position output
		Entity->Add<Print>(Entity->New())->Text = [=]()-> string{
			uint64_t id = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"))->Person;
			if(!id) return "";
			vec3 position = Entity->Get<Form>(id)->Position();
			float height = Entity->Get<Person>(id)->Height;
			return "position  X " + to_string((int)position.x) + " Y " + to_string((int)(position.y - height/2 + 0.01f)) + " Z " + to_string((int)position.z);
		};
	});
}
