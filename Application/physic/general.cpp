#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>

#include "form.h"


void ModulePhysic::Init()
{
	broadphase = new btDbvtBroadphase();
	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	solver = new btSequentialImpulseConstraintSolver;
	auto world = Global->Add<btDiscreteDynamicsWorld>("world", new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration));

	world->setGravity(btVector3(0, -9.81f, 0));
	world->setDebugDrawer(new ModulePhysic::DebugDrawer(Entity, Global, File, Debug));

	Callbacks();
	Listeners();
}

ModulePhysic::~ModulePhysic()
{
	Global->Delete("world");
	delete solver;
	delete dispatcher;
	delete configuration;
	delete broadphase;
}

void ModulePhysic::Update()
{
	auto world = Global->Get<btDiscreteDynamicsWorld>("world");

	auto tfs = Entity->Get<Form>();
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
	{
		if(!i->second->Added)
		{
			world->addRigidBody(i->second->Body);
			i->second->Added = true;
		}
	}

	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);

	world->debugDrawWorld();
}

void ModulePhysic::Listeners()
{
	Event->Listen("InputBindDebugdraw", [=]{
		auto world = Global->Get<btDiscreteDynamicsWorld>("world");

		if(world->getDebugDrawer()->getDebugMode() == btIDebugDraw::DBG_NoDebug)
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		else
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
	});
}
