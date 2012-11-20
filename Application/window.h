#pragma once

#include "system.h"

#include <SFML/Window.hpp>
using namespace sf;


struct StorageWindow : public Storage
{
	Window Window;
};
