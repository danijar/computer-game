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

	// members
	sf::Clock clock;
	float delta;
	//float speed;

	// init
	unsigned int Personrcamera(glm::vec3 Position = glm::vec3(0));

	// state
	bool focus;
	void State();
	void State(bool Active);

	// conrol
	void Rotate(glm::vec3 Amount, float Speed = 0.005f);
	void Move(glm::vec3 Amount, float Speed = 10.0f);

	// calculation
	void Projection();
	void Projection(sf::Vector2u Size);
	void Calculate();
};
