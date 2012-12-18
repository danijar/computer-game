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
		stg->Size       = Vector2<int>(800, 600);
		stg->Title      = "Graphics Application";

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