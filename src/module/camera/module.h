#pragma once

#include "system/system.h"

#include <sfml/System.hpp>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class ModuleCamera : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// state
	void State();
	void State(bool Active);
	bool Focus();

	// conrol
	void Rotate(glm::vec3 Amount, float Sensitivity = 0.0012f);

	// synchronization
	float Yaw();

	// calculate
	void Projection();
	void Projection(sf::Vector2u Size);
	void Calculate();

	// save
	void Load();
	void Save();

public:
	// callbacks
	static void jsCamera(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsFov   (const v8::FunctionCallbackInfo<v8::Value> &args);
};
