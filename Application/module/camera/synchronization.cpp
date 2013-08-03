#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>

#include "form.h"


float ModuleCamera::Yaw()
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<Form>(id);

	// fetch current rotation
	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion rotation = transform.getRotation();

	// extract yaw amount
	btVector3 lookat = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	float yaw = acosf(forward.getZ()) * (forward.getX() < 0 ? -1 : 1);

	return yaw;
}
