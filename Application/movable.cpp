#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "movable.h"
#include "transform.h"


class ComponentMovable : public Component
{
	Clock clock;

	void Init()
	{
		// ...
	}

	void Update()
	{
		auto mvb = Entity->Get<StorageMovable>();
		float time = clock.getElapsedTime().asSeconds();

		for(auto i = mvb.begin(); i != mvb.end(); ++i)
		{
			auto tsf = Entity->Get<StorageTransform>(i->first);

			vec3 move;
			const float spd = .03f;
			if (Keyboard::isKeyPressed(Keyboard::Up)       || Keyboard::isKeyPressed(Keyboard::W)) move += vec3(-spd, -spd,   0 );
			if (Keyboard::isKeyPressed(Keyboard::Down)     || Keyboard::isKeyPressed(Keyboard::S)) move += vec3( spd,  spd,   0 );
			if (Keyboard::isKeyPressed(Keyboard::Left)     || Keyboard::isKeyPressed(Keyboard::A)) move += vec3( spd, -spd,   0 );
			if (Keyboard::isKeyPressed(Keyboard::Right)    || Keyboard::isKeyPressed(Keyboard::D)) move += vec3(-spd,  spd,   0 );
			if (Keyboard::isKeyPressed(Keyboard::PageUp)   || Keyboard::isKeyPressed(Keyboard::Q)) move += vec3(  0,    0,   spd);
			if (Keyboard::isKeyPressed(Keyboard::PageDown) || Keyboard::isKeyPressed(Keyboard::E)) move += vec3(  0,    0,  -spd);
			tsf->Position += move;
		}
	}
};