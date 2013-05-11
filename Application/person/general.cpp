#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/glm.hpp>
#include <SFML/Window.hpp>
using namespace glm;
using namespace sf;

#include "form.h"
#include "person.h"
#include "camera.h"


void ModulePerson::Init()
{
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
	vec3 move;
	if (Keyboard::isKeyPressed(Keyboard::Up      ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
	if (Keyboard::isKeyPressed(Keyboard::Down    ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
	if (Keyboard::isKeyPressed(Keyboard::Left    ) || Keyboard::isKeyPressed(Keyboard::A)) move.z++;
	if (Keyboard::isKeyPressed(Keyboard::Right   ) || Keyboard::isKeyPressed(Keyboard::D)) move.z--;
	if(length(move) > 0 && (psn->Onground || Keyboard::isKeyPressed(Keyboard::LShift)))
		Keyboard::isKeyPressed(Keyboard::LShift) ? Move(id, move, 20.0f) : Move(id, move);
}

void ModulePerson::Listeners()
{
	Event->Listen("InputBindJump", [=] {
		unsigned int id = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person;
		auto psn = Entity->Get<Person>(id);

		if(Keyboard::isKeyPressed(Keyboard::LShift))
		{
			auto tsf = Entity->Get<Form>(id);
			tsf->Body->applyCentralImpulse(btVector3(0, 1000, 0));
		}
		else if(psn->Onground)
		{
			auto tsf = Entity->Get<Form>(id);
			tsf->Body->applyCentralImpulse(btVector3(0, 5.5f, 0) * psn->Mass);
		}
	});
}
