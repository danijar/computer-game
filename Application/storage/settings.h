#pragma once

#include <string>
#include <SFML/System.hpp>


struct Settings
{
	Settings() : Fullscreen(false), Mouse(true), Size(640, 480), Title(""), FPS(0), Fieldofview(45.f), Viewdistance(100.f), Wireframe(false), Verticalsync(true) {}

	// settings
	int FPS;

	// window
	bool Fullscreen;
	bool Mouse;
	sf::Vector2i Position;
	sf::Vector2u Size;
	std::string Title;

	// renderer
	float Fieldofview, Viewdistance;
	bool Wireframe, Verticalsync;
};
