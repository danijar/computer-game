#pragma once

#include "system.h"

#include <string>


class ModuleConsole : public Module
{
	void Init();
	void Update();
	void Listeners();

	std::string text;
	bool active;
};
