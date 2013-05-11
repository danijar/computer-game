#pragma once

#include "system.h"

#include <SFML/Graphics.hpp>


class ModuleInterface : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// drawing
	sf::Font font;
	void Draw();
};
