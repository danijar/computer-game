#pragma once

#include "system.h"


class ModuleWindow : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// create
	void Create();
	void Create(bool Fullscreen);
	void Close();
};
