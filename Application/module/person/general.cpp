#include "module.h"

#include <string>
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <sfml/Window.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "form.h"
#include "person.h"
#include "camera.h"
#include "print.h"


void ModulePerson::Init()
{
	Listeners();


	/*
	// test data manager
	unordered_map<string, string> structure;
	structure.insert(make_pair("mass",    "FLOAT"));
	structure.insert(make_pair("height",  "FLOAT"));
	structure.insert(make_pair("jumping", "BOOLEAN"));
	Data->Register<Person>(structure, [](void *Instance){
		unordered_map<string, string> serialized;
		auto psn = (Person*)Instance;
		serialized.insert(make_pair("mass",    to_string(psn->Mass)));
		serialized.insert(make_pair("height",  to_string(psn->Height)));
		serialized.insert(make_pair("jumping", to_string(psn->Jumping)));
		return serialized;
	}, [](void *Instance, unordered_map<string, string> Serialized){
		auto psn = (Person*)Instance;
		if(Serialized.find("mass")    == Serialized.end()) return false;
		if(Serialized.find("height")  == Serialized.end()) return false;
		if(Serialized.find("jumping") == Serialized.end()) return false;
		psn->Mass    = std::stof(Serialized["mass"]);
		psn->Height  = std::stof(Serialized["height"]);
		psn->Jumping = (Serialized["jumping"] == "true" ? true : false);
		return true;
	});
	unsigned int id = Entity->New();
	auto psn = Entity->Add<Person>(id);
	Data->Save(id, psn);
	Data->Load(id, psn);
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
	auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
	unsigned int id = cam->Person;

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
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
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
			unsigned int id = Entity->New();
			auto psn = Entity->Add<Person>(id);
			auto frm = Entity->Add<Form>(id);
			
			vec3 position = Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position() - vec3(0, psn->Eyes, 0);
			psn->Calculate(1.80f);
			frm->Position(position);

			Debug->Pass("none loaded, added default one");
		}

		// if there is no person bound to camera, bound the first one
		auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
		if(!cam->Person)
		{
			unsigned int id = Entity->Get<Person>().begin()->first;
			cam->Person = id;
			Debug->Pass("not bound to camera, fallback to first one");
		}

		// add player position output
		Entity->Add<Print>(Entity->New())->Text = [=]()-> string{
			unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
			if(!id) return "";
			vec3 position = Entity->Get<Form>(id)->Position();
			float height = Entity->Get<Person>(id)->Height;
			return "position  X " + to_string((int)position.x) + " Y " + to_string((int)(position.y - height/2 + 0.01f)) + " Z " + to_string((int)position.z);
		};
	});
}
