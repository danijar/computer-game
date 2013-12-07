#pragma once

#include "system/system.h"

#include "dependency/glm/glm.hpp"
#include <bullet/btBulletDynamicsCommon.h>

#include "type/form/type.h"


class ModulePerson : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// create
	void Setup(uint64_t Id);

	// movement
	void Move(uint64_t Id, glm::vec3 Amount, float Multiplier = 4.3f);
	void Jump(uint64_t Id, float Multiplier = 4.8f, bool Force = false);

	// checks
	bool Ground(uint64_t Id);
	bool Edge(uint64_t Id, glm::vec3 Direction);
	std::pair<bool, float> RayDown(btVector3 &Position, float Length);
	bool Ray(btVector3 &From, btVector3 &To, btVector3 &Point = btVector3(), btVector3 &Normal = btVector3());

	// save
	void Load();
	void Save();

public:
	// callbacks
	static v8::Handle<v8::Value> jsPlayer(const v8::Arguments& args);
};
