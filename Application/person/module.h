#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <GLM/glm.hpp>


class ModulePerson : public Module
{
	void Init();
	void Update();
	void Listeners();
};
