#include "module.h"

#include <bullet/btBulletDynamicsCommon.h>
#include "dependency/glm/glm.hpp"
using namespace glm;

#include "type/person/type.h"
#include "type/form/type.h"
#include "type/camera/type.h" // get rid of that later


void ModulePerson::Move(uint64_t Id, vec3 Amount, float Speed)
{
	auto frm = Entity->Get<Form>(Id);
	auto psn = Entity->Get<Person>(Id);

	// fetch orientation of person
	btQuaternion rotation = Entity->Get<Form>(*Global->Get<uint64_t>("camera"))->Body->getWorldTransform().getRotation();
	/*
	 * Do not move in camera direction but in person direction.
	 * This doesn't work at the moment. Direction is always positive, though it doesn't work on the half of the map.
	 * btQuaternion rotation = Entity->Get<Form>(Id)->Body->getWorldTransform().getRotation();
	 */

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// adapt to surface angle
	float distance = 0;
	auto result = RayDown(frm->Body->getWorldTransform().getOrigin() + forward * psn->Radius + btVector3(0, - psn->Height/2 + psn->Step, 0), 2 * psn->Step);
	if(result.first) distance = -1 * (result.second - psn->Step);
	Amount.y += distance;

	// sum walking orientations together
	btVector3 current  = frm->Body->getLinearVelocity();
	btVector3 velocity = btVector3(forward * Amount.x + up * Amount.y + side * Amount.z) * Speed;
	if(abs(velocity.getY()) < 0.01f || velocity.getY() < current.getY()) velocity.setY(current.getY());

	// set velocity to move body
	frm->Body->setLinearVelocity(velocity);
}

void ModulePerson::Jump(uint64_t Id, float Multiplier, bool Force)
{
	auto frm = Entity->Get<Form>(Id);
	auto psn = Entity->Get<Person>(Id);

	// jump by applying impulse, sadly a downward impulse to also push the ground doesn't work
	if(!psn->Jumping || Force)
	{
		if(!Force)
		{
			btVector3 velocity = frm->Body->getLinearVelocity();
			frm->Body->setLinearVelocity(btVector3(velocity.getX(), 0, velocity.getZ()));
		}
		float amount = psn->Mass * Multiplier;
		frm->Body->applyCentralImpulse(btVector3(0, amount, 0));
		psn->Jumping = true;
	}
}
