#pragma once

#include "system.h"

#include <SFML/System/Clock.hpp>
using namespace sf;


struct StorageAnimation  : public Storage
{
	StorageAnimation() : Active(false), Lastheight(0.f) {}
	void Start() { Active = true; Starttime.restart(); }
	Clock Starttime;
	bool Active;
	float Lastheight;
};