#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>
using namespace sf;


struct StorageAnimation
{
	StorageAnimation() : Active(false), Lastheight(0.f) {}
	Clock Starttime;
	bool Active;
	float Lastheight;
};