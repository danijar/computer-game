#include "module.h"

#include <cstdlib>
#include <SFML/System/Clock.hpp>
#include <GLM/glm.hpp>
using namespace glm;

#include "movement.h"
#include "transform.h"


void ModuleMovement::Init()
{

}

void ModuleMovement::Update()
{
	float delta = clock.restart().asSeconds();
	auto mvs = Entity->Get<StorageMovement>();

	for(auto i = mvs.begin(); i != mvs.end(); ++i)
	{
		auto tsf = Entity->Get<StorageTransform>(i->first);

		if(i->second->Type == StorageMovement::RANDOM)
			i->second->Direction += vec3(Randf(), Randf(), Randf());
		else if(i->second->Type == StorageMovement::PLANAR)
			i->second->Direction += vec3(Randf(), 0, Randf());

		const float speed = 1.f;
		tsf->Position += i->second->Direction * speed * delta;
	}
}

float ModuleMovement::Randf()
{
	return ((float)rand() - (RAND_MAX/2)) / (RAND_MAX/2);
}
