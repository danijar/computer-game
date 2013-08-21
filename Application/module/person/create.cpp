#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>

#include "form.h"
#include "person.h"


void ModulePerson::Setup(unsigned int Id)
{
	auto psn = Entity->Get<Person>(Id);
	auto frm = Entity->Check<Form>(Id) ? Entity->Get<Form>(Id) : Entity->Add<Form>(Id);

	if(Form::World && frm->Body->isInWorld())
		Form::World->removeRigidBody(frm->Body);

	delete frm->Body->getCollisionShape();
	btCollisionShape *shape = new btCapsuleShape(psn->Radius, psn->Height / 2 /* plus twice radius tall */);
	frm->Body->setCollisionShape(shape);

	btVector3 inertia;
	shape->calculateLocalInertia(psn->Mass, inertia);
	frm->Body->setMassProps(psn->Mass, inertia);

	frm->Body->setAngularFactor(btVector3(0, 1, 0));       // lock rotation around X and Z to prevent falling over
	frm->Body->setFriction(5.0f);                          // set high friction to not slide after walking
	frm->Body->setRollingFriction(5.0f);                   // and to not spin too easily
	frm->Body->setAnisotropicFriction(btVector3(1, 0, 1)); // disable friction in Y direction to not scratch in jumps
	frm->Body->setActivationState(DISABLE_DEACTIVATION);

	if(Form::World)
		Form::World->addRigidBody(frm->Body);
}
