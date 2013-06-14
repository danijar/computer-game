#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <GLM/glm.hpp>


class ModuleCamera : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// create
	unsigned int Create(glm::vec3 Position = glm::vec3(0), float Height = 2.0f);

	// state
	bool focus;
	void State();
	void State(bool Active);
	bool Focus();

	// conrol
	float campitch;
	void Rotate(glm::vec3 Amount, float Sensitivity = 0.0012f);

	// synchronization
	float Yaw();

	// calculate
	void Projection();
	void Projection(sf::Vector2u Size);
	void Calculate();

public:
	// callbacks
	static v8::Handle<v8::Value> jsCamera(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsPerson(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsFov   (const v8::Arguments& args);
};
