#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <SFML/System/Vector2.hpp>
using namespace sf;


struct StorageSettings : public Storage
{
	StorageSettings() : FPS(0) {}

	bool Fullscreen;
	bool Mouse;

	Vector2i Position;
	Vector2u Size;

	string Title;
	int FPS;
};