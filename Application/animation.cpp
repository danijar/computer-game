#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/System/Clock.hpp>
using namespace sf;

#include "animation.h"
#include "transform.h"


class ComponentAnimation : public Component
{
	Clock clock;

	void Init()
	{
		Listeners();
	}

	void Update()
	{
		float time = clock.getElapsedTime().asSeconds();

		auto ams = Entity->Get<StorageAnimation>();
		for(auto i = ams.begin(); i != ams.end(); ++i)
		{
			// rotation
			auto tsf = Entity->Get<StorageTransform>(i->first);
			tsf->Rotation.z = time * 360;

			// jump
			if(i->second->Active)
			{
				auto tsf = Entity->Get<StorageTransform>(i->first);
				const float hgt = .3f;
				const float spd = 10.f;

				float Height = sin(i->second->Starttime.getElapsedTime().asSeconds() * spd);
				if(Height < 0)
				{
					i->second->Active = false;
					Height = 0;
					i->second->Lastheight = 0;
				}
				tsf->Position += vec3(0, 0, (Height - i->second->Lastheight) * hgt);
				i->second->Lastheight = Height;
			}
		}
	}

	void Listeners()
	{
		Event->Listen("InputBindJump", [=]{
			auto ams = Entity->Get<StorageAnimation>();
			for(auto i = ams.begin(); i != ams.end(); ++i)
			{
				i->second->Start();
			}
			Debug::Pass("Animation jump");
		});
	}
};