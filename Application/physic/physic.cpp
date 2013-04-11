#include "module.h"


void ModulePhysic::Ground()
{
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyInfo(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	ground = new btRigidBody(groundRigidBodyInfo);
	world->addRigidBody(ground);
}

void ModulePhysic::Cube()
{
	shapeHeight = 30.f;
	btCollisionShape* fallShape = new btSphereShape(1);
	btVector3 shapePosition(0, shapeHeight, 0);
	btScalar shapeMass = 1;
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, shapeMass), shapePosition));
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(shapeMass, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyInfo(shapeMass, fallMotionState, fallShape, fallInertia);
	shape = new btRigidBody(fallRigidBodyInfo);
	world->addRigidBody(shape);
}
