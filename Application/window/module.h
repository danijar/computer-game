#pragma once

#include "system.h"

#include <SFML/System.hpp>


class ModuleWindow : public Module
{
	// general
	sf::Clock clock;
	void Init();
	void Update();
	void Listeners();

	// window
	unsigned int window;
	int frames;
	void Create();
	void Create(bool Fullscreen);
	void Close();
};
