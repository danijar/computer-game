#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <SFML/System/Vector2.hpp>
using namespace sf;

struct StorageSettings : public Storage
{
	bool Fullscreen;
	Vector2i Position;
	Vector2i Size;
	string Title;
};