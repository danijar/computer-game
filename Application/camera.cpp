#pragma once

#include "system.h"

#include <SFML/Window.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/constants.hpp>
using namespace glm;

#include "window.h"
#include "camera.h"

class ComponentCamera : public Component
{
	Clock clock;
	float duration;

	void Init()
	{
		Global->Add<StorageCamera>("camera");

		Calculate();

		Active();

		Listeners();
	}

	void Update()
	{
		auto cam = Global->Get<StorageCamera>("camera");
		if(!cam->Active) return;
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		duration = clock.restart().asSeconds();
		
		Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
		Vector2i position = Mouse::getPosition(*wnd);
		if(position != center)
		{
			Mouse::setPosition(center, *wnd);
			Vector2i delta = position - center;
			Rotate(delta);
		}

		vec3 move;
		if (Keyboard::isKeyPressed(Keyboard::Up      ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
		if (Keyboard::isKeyPressed(Keyboard::Down    ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
		if (Keyboard::isKeyPressed(Keyboard::Right   ) || Keyboard::isKeyPressed(Keyboard::D)) move.z++;
		if (Keyboard::isKeyPressed(Keyboard::Left    ) || Keyboard::isKeyPressed(Keyboard::A)) move.z--;
		if (Keyboard::isKeyPressed(Keyboard::PageUp  ) || Keyboard::isKeyPressed(Keyboard::Q)) move.y++;
		if (Keyboard::isKeyPressed(Keyboard::PageDown) || Keyboard::isKeyPressed(Keyboard::E)) move.y--;
		Move(move);

		Calculate();
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			switch(Code)
			{
			case Keyboard::Key::F1:
				Active(!Global->Get<StorageCamera>("camera")->Active);
				break;
			}
		});

		Event->Listen("WindowResize", [=]{
			 Refresh();
		});

		Event->Listen("WindowRecreated", [=]{
			 Refresh();
		});
	}

	void Refresh()
	{
		auto cam = Global->Get<StorageCamera>("camera");
		if(cam->Active) Active();
	}

	void Active(bool Active = true)
	{
		auto cam = Global->Get<StorageCamera>("camera");
		auto wnd = &Global->Get<StorageWindow>("window")->Window;

		cam->Active = Active;
		wnd->setMouseCursorVisible(!Active);

		Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
	}

	void Rotate(Vector2i Amount)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float speed = .08f;
		cam->Angles += vec2(-Amount.x, -Amount.y) * speed * duration;
	}

	void Move(vec3 Amount)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		vec3 forward = vec3(sinf(cam->Angles.x), 0, cosf(cam->Angles.x));
		vec3 right = vec3(-forward.z, 0, forward.x);

		const float speed = 10.f;
		cam->Position   += forward * Amount.x * speed * duration;
		cam->Position.y +=           Amount.y * speed * duration;
		cam->Position   += right   * Amount.z * speed * duration;
	}

	void Calculate()
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float pi = glm::pi<float>();
		if		(cam->Angles.x < -pi)	cam->Angles.x += pi*2;
		else if	(cam->Angles.x >  pi)	cam->Angles.x -= pi*2;
 
		const float margin = .2f;
		if		(cam->Angles.y < -pi/2+margin)	cam->Angles.y = -pi/2+margin;
		else if	(cam->Angles.y >  pi/2-margin)	cam->Angles.y =  pi/2-margin;

		vec3 lookat(
			sinf(cam->Angles.x) * cosf(cam->Angles.y),
			                      sinf(cam->Angles.y),
			cosf(cam->Angles.x) * cosf(cam->Angles.y)
		);

		cam->View = lookAt(cam->Position, cam->Position + lookat, vec3(0, 1, 0));
	}

};