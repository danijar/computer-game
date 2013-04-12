#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;


btRigidBody *ModuleModel::CreateBody()
{
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btScalar mass = 1;
	btVector3 inertia(0, 0, 0);
	btCollisionShape* shape = new btSphereShape(1);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, inertia);
	return new btRigidBody(info);
}

btRigidBody *ModuleModel::CreateBodyCube()
{
	btRigidBody *body = CreateBody();
	const btScalar mass = 5;
	btCollisionShape* shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	btVector3 inertia;
	shape->calculateLocalInertia(mass, inertia);
	body->setCollisionShape(shape);
	body->setMassProps(mass, inertia);
	return body;
}

btRigidBody *ModuleModel::CreateBodyPlane()
{
	btRigidBody *body = CreateBody();
	btCollisionShape* shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	body->setCollisionShape(shape);
	body->setMassProps(0, btVector3(0, 0, 0));
	return body;
}
