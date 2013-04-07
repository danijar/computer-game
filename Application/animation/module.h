#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>


class ModuleAnimation : public Module
{
	sf::Clock clock;
	void Init();
	void Update();
	void Listeners();

	void Start(unsigned int id);
};
