#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <GLM/glm.hpp>


class ModuleCamera : public Module
{
	sf::Clock clock;
	float delta;
	float speed;

	void Init();
	void Update();
	void Listeners();

	unsigned int Personrcamera(glm::vec3 Position = glm::vec3(0));

	bool focus;
	void State();
	void State(bool Active);

	void Projection();
	void Projection(sf::Vector2u Size);

	void Rotate(sf::Vector2i Amount, float Speed = 10.0f);
	void Move(glm::vec3 Amount, float Speed = 10.0f);
	void Calculate();
};
