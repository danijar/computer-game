#pragma once

#include "system.h"

#include <sfml/System.hpp>
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
	static v8::Handle<v8::Value> jsCamera(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPerson(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsFov   (const v8::Arguments& args);
};
