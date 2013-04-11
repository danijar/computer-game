#include "module.h"

#include <string>
using namespace std;

#include "transform.h"


void ModulePhysic::Init()
{
	broadphase = new btDbvtBroadphase();
	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration);

	world->setGravity(btVector3(0, -10, 0));

	Ground();
	Cube();
}

void ModulePhysic::Update()
{
	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);
 
	btTransform trans;
	shape->getMotionState()->getWorldTransform(trans);
	float newHeight = trans.getOrigin().getY();
	if(newHeight != shapeHeight)
	{
		shapeHeight = newHeight;
		Debug->Print("shape hight " + to_string(shapeHeight));
	}

	auto tfs = Entity->Get<StorageTransform>();
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
	{
		if(i->second->Changed)
		{
			Matrix(i->first);
			i->second->Changed = false;
		}
		else if(!i->second->Static)
		{
			Matrix(i->first);
		}
	}
}

ModulePhysic::~ModulePhysic()
{
	delete world;
	delete solver;
	delete dispatcher;
	delete configuration;
	delete broadphase;
}
