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
		ivec3 position = (ivec3)Entity->Get<Form>(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person)->Position();
		return "position  X " + to_string(position.x) + " Y " + to_string(position.y) + " Z " + to_string(position.z);
	};

	Listeners();
}

void ModulePerson::Update()
{
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
}

void ModulePerson::Listeners()
{
	Event->Listen("InputBindJump", [=] {
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
		auto psn = Entity->Get<Person>(id);
		auto tsf = Entity->Get<Form>(id);

		if(Keyboard::isKeyPressed(Keyboard::LShift))
			Jump(id, 15.0f, true);
		else if(Ground(id))
			Jump(id);
	});
}
