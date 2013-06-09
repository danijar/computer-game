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

	// fetch person's origin
	btVector3 origin = tsf->Body->getWorldTransform().getOrigin();

	// cast ray at the center
	bool hit = RayDown(origin, psn->Height / 2).first;

	// circularly cast rays until hit
	const int samples = 8;
	for (int i = 0; i < samples && !hit; ++i)
	{
		float value = glm::pi<float>() * 2 * (float)i / samples;
		btVector3 direction = btVector3(sin(value), 0, cos(value)).normalize();
		hit = RayDown(origin + direction * psn->Radius * 0.99f, psn->Height / 2 + GROUND_TOLERANCE + 0.01f).first;
	}
	return hit;
}

bool ModulePerson::Edge(unsigned int Id, vec3 Direction)
{
	auto tsf = Entity->Get<Form>(Id);
	auto psn = Entity->Get<Person>(Id);

	// fetch person's origin and rotation
	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion rotation = transform.getRotation();
	btVector3 origin = transform.getOrigin();

	// create orientation vectors
	btVector3 up(0, 1, 0);
	btVector3 lookat  = quatRotate(rotation, btVector3(0, 0, 1));
	btVector3 forward = btVector3(lookat.getX(), 0, lookat.getZ()).normalize();
	btVector3 side    = btCross(up, forward);

	// get move direction
	btVector3 offset = btVector3(forward * Direction.x + up * Direction.y + side * Direction.z) * (psn->Radius + psn->Step);

	// cast rays to detect edge with air above
	bool hitabove  = RayDown(origin + offset + btVector3(0,  psn->Height/2, 0), psn->Height + GROUND_TOLERANCE).first;
	bool hitbottom = RayDown(origin + offset + btVector3(0, -psn->Height/2, 0), psn->Step   - GROUND_TOLERANCE).first;

	return (hitabove == false && hitbottom == true);
}

pair<bool, float> ModulePerson::RayDown(btVector3 &Position, float Length)
{
	btVector3 Point;
	bool hit = Ray(Position, Position - btVector3(0, Length, 0), Point);
	return make_pair(hit, Position.getY() - Point.getY());
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
