#pragma once

#include "system.h"

#include <SFML/System.hpp>


class ModuleSettings : public Module
{
	// general
	sf::Clock clock;
	int frames;
	void Init();
	void Update();
	void Listeners();
};
