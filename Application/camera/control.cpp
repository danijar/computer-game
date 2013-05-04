#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "camera.h"
#include "transform.h"


void ModuleCamera::Rotate(vec3 Amount, float Speed)
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);

	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion before = transform.getRotation();
	btQuaternion after = before * btQuaternion(Amount.x * Speed, Amount.y * Speed, 0);
	transform.setRotation(after);
	tsf->Body->setWorldTransform(transform);
}

void ModuleCamera::Move(vec3 Amount, float Speed)
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	unsigned int person = Entity->Get<StorageCamera>(camera)->Person;
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(person);

	vec3 forward(tsfcam->Direction().x, 0, tsfcam->Direction().z);
	vec3 right(-forward.z, 0, forward.x);
	vec3 up(0, 1, 0);

	vec3 combined(Amount.x * forward + Amount.y * up + Amount.z * right);
	tsfpsn->Body->setLinearVelocity(btVector3(combined.x, combined.y, combined.z) * Speed);
}
