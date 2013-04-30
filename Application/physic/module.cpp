#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace glm;
using namespace std;

#include "transform.h"
#include "physics.h"


void ModulePhysic::Init()
{
	broadphase = new btDbvtBroadphase();
	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, configuration);

	world->setGravity(btVector3(0, -9.81f, 0));
	world->setDebugDrawer(new ModulePhysic::DebugDrawer(Entity, Global, File, Debug));

	Listeners();
}

ModulePhysic::~ModulePhysic()
{
	delete world;
	delete solver;
	delete dispatcher;
	delete configuration;
	delete broadphase;
}

void ModulePhysic::Update()
{
	auto pys = Entity->Get<StoragePhysic>();
	for(auto i = pys.begin(); i != pys.end(); ++i)
	{
		if(!i->second->InWorld)
		{
			world->addRigidBody(i->second->Body);
			i->second->InWorld = true;
		}
	}

	auto tfs = Entity->Get<StorageTransform>();
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
	{
		auto tsf = i->second;
		if(tsf->Changed)
		{
			if(Entity->Check<StoragePhysic>(i->first))
			{
				auto phy = Entity->Get<StoragePhysic>(i->first);

				btTransform transform = phy->Body->getWorldTransform();
				transform.setOrigin(btVector3(tsf->Position.x, tsf->Position.y, tsf->Position.z));
				transform.setRotation(Rotation(tsf->Rotation));
				phy->Body->setWorldTransform(transform);
				phy->Body->getCollisionShape()->setLocalScaling(btVector3(tsf->Scale.x, tsf->Scale.y, tsf->Scale.z)); // correct orientation?
			}
			Matrix(i->first);
			i->second->Changed = false;
		}
		else if(tsf->Static == false)
		{
			if(Entity->Check<StoragePhysic>(i->first))
			{
				auto phy = Entity->Get<StoragePhysic>(i->first);

				btVector3 position = phy->Body->getWorldTransform().getOrigin();
				tsf->Position = vec3(position.getX(), position.getY(), position.getZ());
				tsf->Rotation = Rotation(phy->Body->getOrientation()); 
			}
			Matrix(i->first);
		}
	}

	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);

	world->debugDrawWorld();
}

void ModulePhysic::Listeners()
{
	Event->Listen("InputBindDebugdraw", [=]{
		if(world->getDebugDrawer()->getDebugMode() == btIDebugDraw::DBG_NoDebug)
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		else
			world->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
	});
}

btQuaternion ModulePhysic::Rotation(vec3 &Angles)
{
	quat quaternion(glm::vec3(Angles.x, Angles.z, Angles.y) * 3.14159f / 180.f);
	return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

vec3 ModulePhysic::Rotation(btQuaternion &Quaternion)
{
	quat quaternion(Quaternion.getW(), -Quaternion.getX(), -Quaternion.getY(), -Quaternion.getZ());
	vec3 angles = eulerAngles(quaternion);
	return vec3(-angles.x, -angles.y, -angles.z);
}
