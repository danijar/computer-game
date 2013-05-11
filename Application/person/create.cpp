#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>

#include "transform.h"
#include "person.h"


void ModulePerson::Setup(unsigned int Id)
{
	auto tsf = Entity->Check<StorageTransform>(Id) ? Entity->Get<StorageTransform>(Id) : Entity->Add<StorageTransform>(Id);
	auto psn = Entity->Get<StoragePerson>(Id);

	delete tsf->Body->getCollisionShape();
	delete tsf->Body;
	btCollisionShape *shape = new btCapsuleShape(psn->Radius, psn->Height / 2 /* plus twice radius tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(psn->Mass, inertia);
	tsf->Body = new btRigidBody(psn->Mass, new btDefaultMotionState(), shape, inertia);

	tsf->Body->setAngularFactor(btVector3(0, 1, 0));       // lock rotation around X and Z to prevent falling over
	tsf->Body->setFriction(5.0f);                          // set high friction to not slide after walking
	tsf->Body->setRollingFriction(5.0f);                   // and to not spin too easily
	tsf->Body->setAnisotropicFriction(btVector3(1, 0, 1)); // disable friction in Y direction to not scratch in jumps
	tsf->Body->setActivationState(DISABLE_DEACTIVATION);
}
