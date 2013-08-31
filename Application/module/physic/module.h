#pragma once

#include "system.h"

#include <sfml/System.hpp>
#include <bullet/btBulletDynamicsCommon.h>


class ModulePhysic : public Module
{
	// general
	sf::Clock clock;
	btBroadphaseInterface               *broadphase;
	btDefaultCollisionConfiguration     *configuration;
	btCollisionDispatcher               *dispatcher;
	btSequentialImpulseConstraintSolver *solver;
	void Init();
	~ModulePhysic();
	void Update();
	void Listeners();
	void Callbacks();

	// debug
	class DebugDrawer : public btIDebugDraw
	{
	public:
		DebugDrawer(ManagerEntity *Entity, ManagerGlobal *Global, HelperFile *File, HelperDebug *Debug);
		void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
		void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {}
		void reportErrorWarning(const char *warningString) {}
		void draw3dText(const btVector3 &location, const char *textString) {}
		void setDebugMode(int debugMode);
		int	getDebugMode() const;
	private:
		ManagerEntity *Entity; ManagerGlobal *Global; HelperFile *File; HelperDebug *Debug;
		GLuint shader;
		int mode;
	};

public:
	// callbacks
	static v8::Handle<v8::Value> jsImpulse (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsForce   (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsVelocity(const v8::Arguments& args);
};
