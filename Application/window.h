#pragma once

#include "system.h"

//#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;


struct StorageWindow : public Storage
{
	RenderWindow Window;
};
