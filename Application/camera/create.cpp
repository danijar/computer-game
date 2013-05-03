#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "camera.h"
#include "person.h"
#include "physic.h"


unsigned int ModuleCamera::Personrcamera(vec3 Position)
{
	unsigned int camera = Entity->New(), person = Entity->New();

	Entity->Add<StorageTransform>(camera)->Position = Position;
	Entity->Add<StorageCamera>(camera)->Person = person;

	Entity->Add<StorageTransform>(person)->Position = Position;
	Entity->Add<StoragePerson>(person);
	auto bdy = Entity->Add<StoragePhysic>(person);

	float mass = 70.0f /* kilogramms mass */;
	btCollisionShape *shape = new btCapsuleShape(0.5f /* meter wide */, 2.0f /* meters tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(mass, inertia);
	Debug->Print("intertia of person is (" + to_string(inertia.getX()) + "," + to_string(inertia.getY()) + "," + to_string(inertia.getZ()));
	bdy->Body = new btRigidBody(mass, new btDefaultMotionState(), shape, inertia);
	bdy->Body->setAngularFactor(btVector3(0, 1, 0)); /* lock rotation around X and Z */

	return camera;
}
