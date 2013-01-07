#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>
using namespace sf;

#include "settings.h"


class ComponentSettings : public Component
{
	int Frames;
	Clock clock;

	void Init()
	{
		auto stg = Global->Add<StorageSettings>("settings");

		stg->Fullscreen = false;
		stg->Mouse		= false;
		stg->Size       = Vector2u(800, 600);
		stg->Title      = "Graphics Application: Terrain";
		stg->Fieldofview  = 45.f;
		stg->Viewdistance = 1000.f;

		Frames = 0;
	}

	void Update()
	{
		auto stg = Global->Get<StorageSettings>("settings");

		if(clock.getElapsedTime().asMilliseconds() >= 1000)
		{
			clock.restart();
			stg->FPS = Frames;
			Frames = 0;
		}
		Frames++;
	}
};