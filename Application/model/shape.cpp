#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;


btRigidBody *ModuleModel::CreateBody(string Path, float Mass)
{
	btCollisionShape *shape = GetShape(Path, (Mass == 0));
	btRigidBody *body = new btRigidBody(Mass, new btDefaultMotionState(), shape);

	if(Mass == 0)
	{
		body->setMassProps(0, btVector3(0, 0, 0));
	}
	else
	{
		btVector3 inertia;
		shape->calculateLocalInertia(Mass, inertia);
		body->setMassProps(Mass, inertia);
	}

	return body;
}

btCollisionShape *ModuleModel::GetShape(string Path, bool Static)
{
	if(Static)
	{
		auto i = staticshapes.find(Path);
		if(i != staticshapes.end()) return i->second;
	}
	else
	{
		auto i = dynamicshapes.find(Path);
		if(i != dynamicshapes.end()) return i->second;
	}

	btCollisionShape *shape = NULL;
	LoadShape(shape, Path, Static);

	if(Static) staticshapes.insert(make_pair(Path, shape));
	else      dynamicshapes.insert(make_pair(Path, shape));

	return shape;
}

void ModuleModel::LoadShape(btCollisionShape *&Shape, string Path, bool Static)
{
	delete Shape;
	Shape = NULL;

	if(Path == "qube.prim")  return LoadShapeCube (Shape);
	if(Path == "plane.prim") return LoadShapePlane(Shape);

	if(Static)
	{

	}
	else
	{

	}
}

void ModuleModel::LoadShapeCube(btCollisionShape *&Shape)
{
	Shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
}

void ModuleModel::LoadShapePlane(btCollisionShape *&Shape)
{
	Shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
}
