#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>


class ModuleMovement : public Module
{
	// general
	sf::Clock clock;
	void Init();
	void Update();

	// helper
	float Randf();
};
