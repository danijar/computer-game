#pragma once

#include "system.h"
#include "debug.h"

#include <cstdlib>
using namespace std;
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "movement.h"
#include "transform.h"


class ModuleMovement : public Module
{
	Clock clock;
	float delta;

	void Init()
	{

	}

	void Update()
	{
		delta = clock.restart().asSeconds();
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

	float Randf()
	{
		return ((float)rand() - (RAND_MAX/2)) / (RAND_MAX/2);
	}
};