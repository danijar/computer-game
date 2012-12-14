#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "form.h"
#include "movable.h"


class ComponentForm : public Component
{
	void Init()
	{
		auto mvs = Storage->Add<StorageMovables>("movables");
	}

	void Update()
	{
		auto mvb = Storage->Get<StorageMovables>("movables");
		auto fms = Storage->Get<StorageForms>("forms");
		float time = clock.getElapsedTime().asSeconds();

		vec3 move;
		const float spd = .03f;
		if (Keyboard::isKeyPressed(Keyboard::Up)       || Keyboard::isKeyPressed(Keyboard::W)) move += vec3(-spd, -spd,   0 );
		if (Keyboard::isKeyPressed(Keyboard::Down)     || Keyboard::isKeyPressed(Keyboard::S)) move += vec3( spd,  spd,   0 );
		if (Keyboard::isKeyPressed(Keyboard::Left)     || Keyboard::isKeyPressed(Keyboard::A)) move += vec3( spd, -spd,   0 );
		if (Keyboard::isKeyPressed(Keyboard::Right)    || Keyboard::isKeyPressed(Keyboard::D)) move += vec3(-spd,  spd,   0 );
		if (Keyboard::isKeyPressed(Keyboard::PageUp)   || Keyboard::isKeyPressed(Keyboard::Q)) move += vec3(  0,    0,   spd);
		if (Keyboard::isKeyPressed(Keyboard::PageDown) || Keyboard::isKeyPressed(Keyboard::E)) move += vec3(  0,    0,  -spd);
		for(auto i = mvb->List.begin(); i != mvb->List.end(); ++i)
		{
			i->Position += move;
		}
		
	}

	Clock clock;

};