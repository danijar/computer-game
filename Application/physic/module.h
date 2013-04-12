#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <BULLET/btBulletDynamicsCommon.h>


class ModulePhysic : public Module
{
	// general
	sf::Clock clock;
	void Init();
	void Update();
	~ModulePhysic();

	// bullet
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* configuration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;

	// transform
	void Matrix(unsigned int id);
};
