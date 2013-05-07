#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "camera.h"
#include "transform.h"
#include "person.h"


void ModuleCamera::Rotate(vec3 Amount, float Sensitivity)
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);

	// apply mouse sensitivity
	Amount *= Sensitivity;

	// clamp camera pitch
	const float clamp = 1.0f;
	if     (campitch + Amount.x >  clamp) Amount.x =  clamp - campitch;
	else if(campitch + Amount.x < -clamp) Amount.x = -clamp - campitch;
	campitch += Amount.x;

	// fetch current rotation
	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion rotation = transform.getRotation();

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// rotate camera with quaternions created from axis and angle
	rotation = btQuaternion(up,      Amount.y) * rotation;
	rotation = btQuaternion(side,    Amount.x) * rotation;
	rotation = btQuaternion(forward, Amount.z) * rotation;

	// set new rotation
	transform.setRotation(rotation);
	tsf->Body->setWorldTransform(transform);
}

void ModuleCamera::Move(vec3 Amount, float Speed)
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	unsigned int person = Entity->Get<StorageCamera>(camera)->Person;
	auto tsf = Entity->Get<StorageTransform>(person);

	// fetch camera orientation
	btQuaternion rotation = Entity->Get<StorageTransform>(camera)->Body->getWorldTransform().getRotation();

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// sum walking orientations together
	btVector3 current  = tsf->Body->getLinearVelocity();
	btVector3 velocity = btVector3(forward * Amount.x + up * Amount.y + side * Amount.z) * Speed;
	if(abs(velocity.getY()) < 0.01f) velocity.setY(current.getY());

	// set velocity to move body
	tsf->Body->setLinearVelocity(velocity);
}
