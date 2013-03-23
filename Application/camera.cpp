#pragma once

#include "system.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/constants.hpp>
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "text.h"


class ModuleCamera : public Module
{
	Clock clock;
	float delta;
	float speed;

	void Init()
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto cam = Global->Add<StorageCamera>("camera");
		cam->Position = vec3(0, 5, -5);
		cam->Active = !stg->Mouse;

		Calculate();
		State();
		Projection();

		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			return "X " + to_string((int)floor(cam->Position.x)) + " Y " + to_string((int)floor(cam->Position.y)) + " Z " + to_string((int)floor(cam->Position.z));
		};

		speed = 10.f;

		Listeners();
	}

	void Update()
	{
		auto cam = Global->Get<StorageCamera>("camera");
		auto wnd = Global->Get<RenderWindow>("window");
		delta = clock.restart().asSeconds();

		if(!cam->Active) return;
		
		Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
		Vector2i position = Mouse::getPosition(*wnd);
		if(position != center)
		{
			Mouse::setPosition(center, *wnd);
			Vector2i offset = position - center;
			Rotate(offset);
		}

		vec3 move;
		if (KeyDown(Key::Up      ) || KeyDown(Key::W)) move.x++;
		if (KeyDown(Key::Down    ) || KeyDown(Key::S)) move.x--;
		if (KeyDown(Key::Right   ) || KeyDown(Key::D)) move.z++;
		if (KeyDown(Key::Left    ) || KeyDown(Key::A)) move.z--;
		if (KeyDown(Key::PageUp  ) || KeyDown(Key::Q)) move.y++;
		if (KeyDown(Key::PageDown) || KeyDown(Key::E)) move.y--;
		Move(move, speed * (KeyDown(Key::LShift) ? 10 : 1));

		Calculate();
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto cam = Global->Get<StorageCamera>("camera");

			switch(Code)
			{
			case Keyboard::Key::F1:
				State(!cam->Active);
				break;
			}
		});

		Event->Listen("WindowRecreated", [=]{
			 State();
			 Projection();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			State();
			Projection(Size);
		});
	}

	void State()
	{
		auto cam = Global->Get<StorageCamera>("camera");
		State(cam->Active);
	}

	void State(bool Active)
	{
		auto cam = Global->Get<StorageCamera>("camera");
		auto wnd = Global->Get<RenderWindow>("window");

		cam->Active = Active;
		wnd->setMouseCursorVisible(!Active);

		if(Active) Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
	}

	void Projection()
	{
		Projection(Global->Get<RenderWindow>("window")->getSize());
	}

	void Projection(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto cam = Global->Get<StorageCamera>("camera");

		cam->Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
	}

	void Rotate(Vector2i Amount)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float speed_rotate = .08f;
		cam->Angles += vec2(-Amount.x, -Amount.y) * speed_rotate * delta;
	}

	void Move(vec3 Amount, float speed = 10.f)
	{
		auto cam = Global->Get<StorageCamera>("camera");

		vec3 forward = vec3(sinf(cam->Angles.x), 0, cosf(cam->Angles.x));
		vec3 right = vec3(-forward.z, 0, forward.x);

		cam->Position   += forward * Amount.x * speed * delta;
		cam->Position.y +=           Amount.y * speed * delta;
		cam->Position   += right   * Amount.z * speed * delta;
	}

	void Calculate()
	{
		auto cam = Global->Get<StorageCamera>("camera");

		const float pi = glm::pi<float>();
		if		(cam->Angles.x < -pi)	cam->Angles.x += pi*2;
		else if	(cam->Angles.x >  pi)	cam->Angles.x -= pi*2;
 
		const float margin = 0.2f;
		if		(cam->Angles.y < -pi/2+margin)	cam->Angles.y = -pi/2+margin;
		else if	(cam->Angles.y >  pi/2-margin)	cam->Angles.y =  pi/2-margin;

		vec3 lookat(
			sinf(cam->Angles.x) * cosf(cam->Angles.y),
			                      sinf(cam->Angles.y),
			cosf(cam->Angles.x) * cosf(cam->Angles.y)
		);

		cam->View = lookAt(cam->Position, cam->Position + lookat, vec3(0, 1, 0));
	}

	typedef Keyboard::Key Key;
	bool KeyDown(Keyboard::Key key) { return Keyboard::isKeyPressed(key); }
};