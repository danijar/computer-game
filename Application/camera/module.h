#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <GLM/glm.hpp>


class ModuleCamera : public Module
{
	// general
	sf::Clock clock;
	void Init();
	void Update();
	void Listeners();

	// create
	unsigned int Create(glm::vec3 Position = glm::vec3(0), float Height = 2.0f);

	// state
	bool focus;
	void State();
	void State(bool Active);
	bool Focus();

	// conrol
	float campitch;
	void Rotate(glm::vec3 Amount, float Time, float Sensitivity = 0.1f);

	// calculate
	void Projection();
	void Projection(sf::Vector2u Size);
	void Calculate();
};
