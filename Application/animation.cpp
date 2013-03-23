#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/System/Clock.hpp>
using namespace sf;

#include "animation.h"
#include "transform.h"


class ModuleAnimation : public Module
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
			tsf->Rotation.y = time * 360;

			// jump
			if(i->second->Active)
			{
				auto tsf = Entity->Get<StorageTransform>(i->first);
				const float maxheight = 1.5f;
				const float speed = 10.f;

				float Height = sin(i->second->Starttime.getElapsedTime().asSeconds() * speed);
				if(Height < 0)
				{
					i->second->Active = false;
					Height = 0;
					i->second->Lastheight = 0;
				}
				tsf->Position += vec3(0, (Height - i->second->Lastheight) * maxheight, 0);
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
				if(!i->second->Active) Start(i->first);
			}
			Debug::Pass("Animation jump");
		});
	}

	void Start(unsigned int id)
	{
		auto amn = Entity->Get<StorageAnimation>(id);
		amn->Active = true;
		amn->Starttime.restart();
	}
};