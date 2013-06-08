#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;

#include "person.h"
#include "form.h"


bool ModulePerson::Ground(unsigned int Id)
{
	auto tsf = Entity->Get<Form>(Id);
	auto psn = Entity->Get<Person>(Id);

	btVector3 origin = tsf->Body->getWorldTransform().getOrigin();

	bool hit = RayDown(origin, psn->Height / 2).first;

	const int samples = 8;
	for (int i = 0; i < samples && !hit; ++i)
	{
		float value = glm::pi<float>() * 2 * (float)i / samples;
		btVector3 direction = btVector3(sin(value), 0, cos(value)).normalize();
		hit = RayDown(origin + direction * psn->Radius * 0.99f, psn->Height / 2 + GROUND_TOLECANRE + 0.01f).first;
	}
	return hit;
}

bool ModulePerson::Edge(unsigned int Id, vec3 Direction)
{
	return false;
}

pair<bool, float> ModulePerson::RayDown(btVector3 &Position, float Length)
{
	btVector3 Point;
	bool hit = Ray(Position, Position - btVector3(0, Length, 0), Point);
	return make_pair(hit, Point.getY() - Position.getY());
}


bool ModulePerson::Ray(btVector3 &From, btVector3 &To, btVector3 &Point, btVector3 &Normal)
{
	auto world = Global->Get<btDiscreteDynamicsWorld>("world");

	btCollisionWorld::ClosestRayResultCallback ray(From, To);
	world->rayTest(From, To, ray);

	if(ray.hasHit())
	{
		Point  = ray.m_hitPointWorld;
		Normal = ray.m_hitNormalWorld;
		return true;
	}
	return false;
}
