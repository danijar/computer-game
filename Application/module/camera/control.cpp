#include "module.h"

#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "camera.h"
#include "form.h"


void ModuleCamera::Rotate(vec3 Amount, float Sensitivity)
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto cam = Entity->Get<Camera>(id);
	auto frm = Entity->Get<Form>(id);

	// apply multipliers
	Amount *= Sensitivity;

	// clamp camera pitch
	const float clamp = radians(85.0f);
	if     (cam->Pitch + Amount.x >  clamp) Amount.x =  clamp - cam->Pitch;
	else if(cam->Pitch + Amount.x < -clamp) Amount.x = -clamp - cam->Pitch;
	cam->Pitch += Amount.x;

	// fetch current rotation
	btTransform transform = frm->Body->getWorldTransform();
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
	frm->Body->setWorldTransform(transform);
}
