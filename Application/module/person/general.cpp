#include "module.h"

#include <string>
#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/glm.hpp>
#include <SFML/Window.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "form.h"
#include "person.h"
#include "camera.h"
#include "print.h"


void ModulePerson::Init()
{
	Entity->Add<Print>(Entity->New())->Text = [=]{
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
		vec3 position = Entity->Get<Form>(id)->Position();
		float height = Entity->Get<Person>(id)->Height;
		return "position  X " + to_string((int)position.x) + " Y " + to_string((int)(position.y - height/2 + 0.01f)) + " Z " + to_string((int)position.z);
	};

	Listeners();
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
	unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
	auto psn = Entity->Get<Person>(id);
	auto tsf = Entity->Get<Form>(id);

	static bool once = true;
	if(once){ once = false; Load(id); } // why get this overwritten when executed on "SystemInitialized"?

	vec3 move;
	if (Keyboard::isKeyPressed(Keyboard::Up   ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
	if (Keyboard::isKeyPressed(Keyboard::Down ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
	if (Keyboard::isKeyPressed(Keyboard::Left ) || Keyboard::isKeyPressed(Keyboard::A)) move.z++;
	if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) move.z--;
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
			tsf->Body->setLinearVelocity(btVector3(0, 0, 0));

		psn->Walking = false;
	}

	if(psn->Jumping && !Keyboard::isKeyPressed(Keyboard::Space) && Ground(id))
		psn->Jumping = false;

	// store player position every some milliseconds
	static Clock clock;
	if(clock.getElapsedTime().asMilliseconds() > 500)
	{
		Save(id);
		clock.restart();
	}
}

void ModulePerson::Listeners()
{
	Event->Listen("InputBindJump", [=]{
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
		auto psn = Entity->Get<Person>(id);
		auto tsf = Entity->Get<Form>(id);

		if(Keyboard::isKeyPressed(Keyboard::LShift))
			Jump(id, 15.0f, true);
		else if(Ground(id))
			Jump(id);
	});

	Event->Listen("SystemInitialized", [=]{
		/*
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;

		if(Form::World && Entity->Get<Form>(id)->Body->isInWorld()) Form::World->removeRigidBody(Entity->Get<Form>(id)->Body);
		Load(id);
		//if(Form::World) Form::World->addRigidBody(Entity->Get<Form>(id)->Body);

		Debug->Print("new x coordinate " + to_string(Entity->Get<Form>(id)->Position().x));
		Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Rotation(Entity->Get<Form>(id)->Rotation());
		*/
	});
}
