#include "module.h"

#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "camera.h"
#include "transform.h"


void ModuleCamera::Rotate(vec3 Amount, float Time, float Sensitivity)
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);

	// apply multipliers
	Amount *= Time;
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
