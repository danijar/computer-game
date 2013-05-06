#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "camera.h"
#include "person.h"


unsigned int ModuleCamera::Create(vec3 Position, float Height)
{
	unsigned int camera = Entity->New(), person = Entity->New();
	auto tsfcam = Entity->Add<StorageTransform>(camera);
	auto tsfpsn = Entity->Add<StorageTransform>(person);
	Entity->Add<StorageCamera>(camera)->Person = person;
	Entity->Add<StoragePerson>(person)->Eyes   = Height/1.1f/2; /* meters to eyes from center */

	tsfcam->Body->setActivationState(DISABLE_DEACTIVATION);
	tsfcam->Position(Position);

	float mass = 70.0f /* kilogramms mass */;
	btCollisionShape *shape = new btCapsuleShape(Height/3 /* meters radius */, Height/3 /* meters plus twice radius tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(mass, inertia);
	
	tsfpsn->Body = new btRigidBody(mass, new btDefaultMotionState(), shape, inertia);
	tsfpsn->Body->setAngularFactor(btVector3(0, 1, 0)); /* lock rotation around X and Z to prevent falling over */
	tsfpsn->Body->setActivationState(DISABLE_DEACTIVATION);
	tsfpsn->Position(Position);

	return camera;
}
