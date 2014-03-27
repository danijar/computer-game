#include "module.h"

#include <bullet/btBulletDynamicsCommon.h>

#include "type/form/type.h"
btDynamicsWorld* Form::World = NULL;


void ModulePhysics::Init()
{
	Opengl->Init();

	broadphase = new btDbvtBroadphase();
	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	solver = new btSequentialImpulseConstraintSolver;
	auto world = Global->Add<btDiscreteDynamicsWorld>("world", new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration));
	world->setDebugDrawer(new ModulePhysics::DebugDrawer(Entity, Global, File, Log));

	world->setGravity(btVector3(0, -9.81f, 0));

	if(!Form::World)
		Form::World = world;

	Callbacks();
	Listeners();
}

ModulePhysics::~ModulePhysics()
{
	Form::World = NULL;
	Global->Delete("world");
	delete solver;
	delete dispatcher;
	delete configuration;
	delete broadphase;
}

void ModulePhysics::Update()
{
	auto world = Global->Get<btDiscreteDynamicsWorld>("world");

	auto tfs = Entity->Get<Form>();
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
		if(!i->second->Body->isInWorld())
			world->addRigidBody(i->second->Body);

	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);

	world->debugDrawWorld();
}

void ModulePhysics::Listeners()
{
	Event->Listen("InputBindDebugdraw", [=]{
		auto world = Global->Get<btDiscreteDynamicsWorld>("world");

		if(world->getDebugDrawer()->getDebugMode() == btIDebugDraw::DBG_NoDebug)
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		else
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
	});
}
