#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace glm;
using namespace std;

#include "transform.h"
#include "physic.h"


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
				transform.setOrigin(Position(tsf->Position));
				transform.setRotation(Rotation(tsf->Rotation));
				phy->Body->setWorldTransform(transform);
				phy->Body->getCollisionShape()->setLocalScaling(btVector3(tsf->Scale.x, tsf->Scale.y, tsf->Scale.z)); // correct orientation?
			}

			if(tsf->Rotation.x < -180) tsf->Rotation.x += 360; else if(tsf->Rotation.x > 180) tsf->Rotation.x -= 360;
			if(tsf->Rotation.y < -180) tsf->Rotation.y += 360; else if(tsf->Rotation.y > 180) tsf->Rotation.y -= 360;
			if(tsf->Rotation.z < -180) tsf->Rotation.z += 360; else if(tsf->Rotation.z > 180) tsf->Rotation.z -= 360;

			Matrix(i->first);
			i->second->Changed = false;
		}
	}

	float delta = clock.restart().asSeconds();
	world->stepSimulation(delta, 10);

	// use motion states for that later on
	for(auto i = tfs.begin(); i != tfs.end(); ++i)
	{
		auto tsf = i->second;
		if(!tsf->Static)
		{
			if(Entity->Check<StoragePhysic>(i->first))
			{
				auto phy = Entity->Get<StoragePhysic>(i->first);

				tsf->Position = Position(phy->Body->getWorldTransform().getOrigin());
				tsf->Rotation = Rotation(phy->Body->getOrientation());
				//tsf->Rotation = Rotation(phy->Body->getWorldTransform().getRotation());
			}
			Matrix(i->first);
		}
	}

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

btVector3 ModulePhysic::Position(vec3 &Coordinates)
{
	return btVector3(Coordinates.x, Coordinates.y, Coordinates.z);
}

vec3 ModulePhysic::Position(btVector3 &Coordinates)
{
	return vec3(Coordinates.getX(), Coordinates.getY(), Coordinates.getZ());
}

btQuaternion ModulePhysic::Rotation(vec3 &Angles)
{
	vec3 angles = radians(Angles); // always use radians instead of degrees
	return btQuaternion(angles.y, angles.x, angles.z);
}

vec3 ModulePhysic::Rotation(btQuaternion &Quaternion)
{
	// this doesn't work properly
	return eulerAngles(quat(Quaternion.getW(), Quaternion.getX(), Quaternion.getY(), Quaternion.getZ()));
}
