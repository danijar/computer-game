#pragma once

#include "system.h"
#include "debug.h"

#include <SFML/System/Clock.hpp>
using namespace sf;

#include "form.h"

class ComponentAnimation : public Component
{
	void Init()
	{

		/*
		Jumping = false;
		Lastheight = 0;

		Listeners();
		*/
	}

	void Update()
	{
		/*
		auto cube = Storage->Get<StorageForm>("cube");

		if(Jumping)
		{
			const float hgt = .3f;
			const float spd = 10.f;

			float Jumpheight = sin(Jumptime.getElapsedTime().asSeconds() * spd);
			if(Jumpheight < 0)
			{
				Jumping = false;
				Jumpheight = 0;
				Lastheight = 0;
			}
			cube->Position += vec3(0, 0, (Jumpheight - Lastheight) * hgt);
			Lastheight = Jumpheight;
		}
		*/

	}
	/*
	Clock Jumptime;
	bool Jumping;
	float Lastheight;

	void Listeners()
	{
		Event->Listen("InputBindJump", [=]{
			Debug::Pass("Animation jump");
			Jumping = true;
			Jumptime.restart();
		});
	}
	*/
};