#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/glm.hpp>
using namespace glm;

#include "person.h"
#include "transform.h"
#include "camera.h" // get rid of that later


void ModulePerson::Move(unsigned int Id, vec3 Amount, float Speed)
{
	auto tsf = Entity->Get<StorageTransform>(Id);
	auto psn = Entity->Get<StoragePerson>(Id);

	// fetch orientation of person
	btQuaternion rotation = Entity->Get<StorageTransform>(*Global->Get<unsigned int>("camera"))->Body->getWorldTransform().getRotation();
	/*
	 * Do not move in camera direction but in person direction.
	 * This doesn't work at the moment. Direction is always positive, to it doesn't work on the half of the map.
	 * btQuaternion rotation = Entity->Get<StorageTransform>(Id)->Body->getWorldTransform().getRotation();
	 */

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// adapt to surface angle
	float distance = 0;
	auto result = RayDown(tsf->Body->getWorldTransform().getOrigin() + forward * psn->Radius + btVector3(0, - psn->Height/2 + psn->Step, 0), 2 * psn->Step);
	if(result.first) distance = result.second + psn->Step;
	Amount.y += distance;

	// sum walking orientations together
	btVector3 current  = tsf->Body->getLinearVelocity();
	btVector3 velocity = btVector3(forward * Amount.x + up * Amount.y + side * Amount.z) * Speed;
	if(abs(velocity.getY()) < 0.01f || velocity.getY() < current.getY()) velocity.setY(current.getY());

	// set velocity to move body
	tsf->Body->setLinearVelocity(velocity);
}

void ModulePerson::Jump(unsigned int Id, float Multiplier)
{
	auto tsf = Entity->Get<StorageTransform>(Id);
	auto psn = Entity->Get<StoragePerson>(Id);

	// jump by applying impulse, sadly a downward impulse to also push the ground doesn't work
	tsf->Body->applyCentralImpulse(btVector3(0, 5.5, 0) * psn->Mass * Multiplier);
}
