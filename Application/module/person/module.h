#pragma once

#include "system.h"

#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include "form.h"


class ModulePerson : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();

	// create
	void Setup(unsigned int Id);

	// movement
	void Move(unsigned int Id, glm::vec3 Amount, float Multiplier = 4.3f);
	void Jump(unsigned int Id, float Multiplier = 4.8f, bool Force = false);

	// checks
	bool Ground(unsigned int Id);
	bool Edge(unsigned int Id, glm::vec3 Direction);
	std::pair<bool, float> RayDown(btVector3 &Position, float Length);
	bool Ray(btVector3 &From, btVector3 &To, btVector3 &Point = btVector3(), btVector3 &Normal = btVector3());

	// save
	void Load();
	void Save();
};
