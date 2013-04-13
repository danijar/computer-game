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
		if(i->second->Changed)
		{
			if(Entity->Check<StoragePhysic>(i->first))
			{
				auto phy = Entity->Get<StoragePhysic>(i->first);

				btTransform transform = phy->Body->getWorldTransform();
				transform.setOrigin(btVector3(i->second->Position.x, i->second->Position.y, i->second->Position.z));
				transform.setRotation(btQuaternion(i->second->Rotation.x, i->second->Rotation.y, i->second->Rotation.z));
				phy->Body->setWorldTransform(transform);
			}
			Matrix(i->first);
			i->second->Changed = false;
		}
		else if(i->second->Static == false)
		{
			if(Entity->Check<StoragePhysic>(i->first))
			{
				auto phy = Entity->Get<StoragePhysic>(i->first);

				btVector3 position = phy->Body->getWorldTransform().getOrigin();
				i->second->Position = vec3(position.getX(), position.getY(), position.getZ());

				btQuaternion orientation = phy->Body->getOrientation();
				quat quaternion(orientation.getW(), -orientation.getX(), -orientation.getY(), -orientation.getZ());
				vec3 rotation = eulerAngles(quaternion);
				i->second->Rotation = vec3(-rotation.x, -rotation.y, -rotation.z); 
			}
			Matrix(i->first);
		}
	}

	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);
}

ModulePhysic::~ModulePhysic()
{
	delete world;
	delete solver;
	delete dispatcher;
	delete configuration;
	delete broadphase;
}
