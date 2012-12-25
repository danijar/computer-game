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
	void Init()
	{
		Global->Add<StorageCamera>("camera");
		Calculate();

		Active();

		Listeners();
	}

	void Update()
	{

	}

	void Listeners()
	{
		Event->Listen<Vector2i>("InputMouseMove", [=](Vector2i Position){
			if(Global->Get<StorageCamera>("camera")->Active)
			{
				auto wnd = &Global->Get<StorageWindow>("window")->Window;
				Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
				if(Position == center) return;
				Mouse::setPosition(center, *wnd);
				Vector2i delta = Position - center;
				Rotate(delta);
			}
		});

		Event->Listen<Keyboard::Key>("InputKeyPressed", [=](Keyboard::Key Code){
			if(Global->Get<StorageCamera>("camera")->Active)
			{
				vec3 move;
				switch(Code)
				{
				case Keyboard::Key::Down:
				case Keyboard::Key::S:
					move.x++;
					break;
				case Keyboard::Key::Up:
				case Keyboard::Key::W:
					move.x--;
					break;
				case Keyboard::Key::Right:
				case Keyboard::Key::D:
					move.z++;
					break;
				case Keyboard::Key::Left:
				case Keyboard::Key::A:
					move.z--;
					break;
				case Keyboard::Key::PageUp:
				case Keyboard::Key::Q:
					move.y++;
					break;
				case Keyboard::Key::PageDown:
				case Keyboard::Key::E:
					move.y--;
					break;
				}
				Move(move);
			}
		});

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

		cam->Active = !cam->Active;
		wnd->setMouseCursorVisible(!cam->Active);

		Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
	}

	void Rotate(Vector2i Amount)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float speed = .0008f;
		cam->Angles += vec2(Amount.x, Amount.y) * speed;

		Calculate();
	}

	void Move(vec2 Amount)
	{
		Move(vec3(Amount.x, 0, Amount.y));
	}

	void Move(vec3 Amount)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		vec3 forward = vec3(sinf(cam->Angles.x), 0, cosf(cam->Angles.x));
		vec3 right = vec3(-forward.z, 0, forward.x);

		const float speed = .1f;
		cam->Position += forward * speed * -Amount.x;
		cam->Position.y += speed * Amount.y;
		cam->Position += right * speed * Amount.z;

		Calculate();
	}

	void Calculate()
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float pi = glm::pi<float>();
		if		(cam->Angles.x < -pi)	cam->Angles.x += pi*2;
		else if	(cam->Angles.x >  pi)	cam->Angles.x -= pi*2;
 
		const float margin = .5f;
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