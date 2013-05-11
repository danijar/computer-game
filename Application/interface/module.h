#pragma once

#include "system.h"

#include <SFML/Graphics.hpp>


class ModuleInterface : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// draw
	sf::Font font;
	void Draw();
};
