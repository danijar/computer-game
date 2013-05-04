#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "camera.h"
#include "person.h"


unsigned int ModuleCamera::Personrcamera(vec3 Position)
{
	unsigned int camera = Entity->New(), person = Entity->New();
	auto tsfcam = Entity->Add<StorageTransform>(camera);
	auto tsfpsn = Entity->Add<StorageTransform>(person);
	Entity->Add<StorageCamera>(camera)->Person = person;
	Entity->Add<StoragePerson>(person);

	tsfcam->Position(Position);
	tsfcam->Body->setActivationState(DISABLE_DEACTIVATION);

	float mass = 70.0f /* kilogramms mass */;
	btCollisionShape *shape = new btCapsuleShape(0.5f /* meter wide */, 2.0f /* meters tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(mass, inertia);

	tsfpsn->Position(Position);
	tsfpsn->Body = new btRigidBody(mass, new btDefaultMotionState(), shape, inertia);
	tsfpsn->Body->setAngularFactor(btVector3(0, 1, 0)); /* lock rotation around X and Z */
	tsfpsn->Body->setActivationState(DISABLE_DEACTIVATION);

	return camera;
}
