#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "camera.h"
#include "transform.h"


void ModuleCamera::Rotate(vec3 Amount, float Sensitivity)
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);

	// fetch current rotation
	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion rotation = transform.getRotation();

	// apply mouse sensitivity
	Amount *= Sensitivity;

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

	// fetch current rotation
	btQuaternion rotation = Entity->Get<StorageTransform>(camera)->Body->getWorldTransform().getRotation();

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// combine movements of all orientations
	btVector3 combined = btVector3(forward * Amount.x + up * Amount.y + side * Amount.z) * Speed;

	// move body by velocity
	Entity->Get<StorageTransform>(person)->Body->setLinearVelocity(combined);
}
