#pragma once

#include "system.h"

#include "settings.h"

class ComponentSettings : public Component
{
	void Init()
	{
		auto stg = Storage->Add<StorageSettings>("settings");
		stg->Fullscreen = false;
		stg->Width      = 800;
		stg->Height     = 600;
		stg->Title      = "Application Title";
	}

	void Update()
	{

	}
};