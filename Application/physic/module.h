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
	
	// test
	btRigidBody* ground;
	btRigidBody* shape;
	float shapeHeight;
	void Ground();
	void Cube();

	// transform
	void Matrix(unsigned int id);
};
