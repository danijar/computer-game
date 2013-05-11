#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/glm.hpp>
#include <SFML/Window.hpp>
using namespace glm;
using namespace sf;

#include "transform.h"
#include "person.h"
#include "camera.h"


void ModulePerson::Init()
{
	Listeners();
}

void ModulePerson::Update()
{
	auto pns = Entity->Get<StoragePerson>();
	
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

	// person attached to active camera
	unsigned int id = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person;
	auto psn = Entity->Get<StoragePerson>(id);

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
		unsigned int id = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person;
		auto psn = Entity->Get<StoragePerson>(id);

		if(Keyboard::isKeyPressed(Keyboard::LShift))
		{
			auto tsf = Entity->Get<StorageTransform>(id);
			tsf->Body->applyCentralImpulse(btVector3(0, 1000, 0));
		}
		else if(psn->Onground)
		{
			auto tsf = Entity->Get<StorageTransform>(id);
			tsf->Body->applyCentralImpulse(btVector3(0, 5.5f, 0) * psn->Mass);
		}
	});
}

void ModulePerson::Setup(unsigned int Id)
{
	auto tsf = Entity->Check<StorageTransform>(Id) ? Entity->Get<StorageTransform>(Id) : Entity->Add<StorageTransform>(Id);
	auto psn = Entity->Get<StoragePerson>(Id);

	delete tsf->Body->getCollisionShape();
	delete tsf->Body;
	btCollisionShape *shape = new btCapsuleShape(psn->Radius, psn->Height / 2 /* plus twice radius tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(psn->Mass, inertia);
	tsf->Body = new btRigidBody(psn->Mass, new btDefaultMotionState(), shape, inertia);

	tsf->Body->setAngularFactor(btVector3(0, 1, 0));       // lock rotation around X and Z to prevent falling over
	tsf->Body->setFriction(5.0f);                          // set high friction to not slide after walking
	tsf->Body->setRollingFriction(5.0f);                   // and to not spin too easily
	tsf->Body->setAnisotropicFriction(btVector3(1, 0, 1)); // disable friction in Y direction to not scratch in jumps
	tsf->Body->setActivationState(DISABLE_DEACTIVATION);
}
