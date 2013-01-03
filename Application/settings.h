#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <SFML/System/Vector2.hpp>
using namespace sf;


struct StorageSettings : public Storage
{
	StorageSettings() : Fullscreen(false), Mouse(true), Size(640, 480), Title(""), FPS(0), Fieldofview(45.f), Viewdistance(100.f), Wireframe(false) {}

	// Window
	bool Fullscreen;
	bool Mouse;
	Vector2i Position;
	Vector2u Size;
	string Title;

	// Debug
	int FPS;

	// Renderer
	float Fieldofview, Viewdistance;
	bool Wireframe;
};