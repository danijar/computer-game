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

	void State();
	void State(bool Active);

	void Projection();
	void Projection(sf::Vector2u Size);

	void Rotate(sf::Vector2i Amount);
	void Move(glm::vec3 Amount, float Speed = 10.f);
	void Calculate();
};
