#pragma once

#include "system.h"

#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>


class ModulePerson : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// setup
	void Setup(unsigned int Id);

	// movement
	void Move(unsigned int Id, glm::vec3 Amount, float Multiplier = 5.0f);
	void Jump(unsigned int Id, float Multiplier = 1.0f);

	// ground
	void Ground(unsigned int Id);
	bool Ray(btVector3 &From, btVector3 &To, btVector3 &Point = btVector3(), btVector3 &Normal = btVector3());
	std::pair<bool, float> RayDown(btVector3 &Position, float Length);
};
