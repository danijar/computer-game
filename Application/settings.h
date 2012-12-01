#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
using namespace sf;


struct StorageSettings : public Storage
{
	bool Fullscreen;
	Vector2i Position;
	Vector2i Size;

	string Title;

	int FPS;
};