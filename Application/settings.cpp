#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>
using namespace sf;

#include "settings.h"


class ComponentSettings : public Component
{
	void Init()
	{
		auto stg = Global->Add<StorageSettings>("settings");

		stg->Fullscreen   = false;
		stg->Mouse		  = false;
		stg->Size         = Vector2u(800, 600);
		stg->Title        = "Graphics Application";
		stg->Fieldofview  = 45.f;
		stg->Viewdistance = 1000.f;
	}

	void Update()
	{

	}
};
