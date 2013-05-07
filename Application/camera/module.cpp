#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/glm.hpp>
using namespace sf;
using namespace glm;

#include "settings.h"
#include "transform.h"
#include "camera.h"
#include "person.h"
#include "text.h"


void ModuleCamera::Init()
{
	unsigned int id = Create(vec3(0, 5, 0), 1.80f);
	*Global->Add<unsigned int>("camera") = id;
	auto cam = Entity->Get<StorageCamera>(id);
	cam->Active = !Global->Get<StorageSettings>("settings")->Mouse;

	campitch = 0.0f;
	focus = true; // how to find out whether window was opened in background?

	Calculate();
	State();
	Projection();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{
		ivec3 position = (ivec3)Entity->Get<StorageTransform>(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person)->Position();
		return "X " + to_string(position.x) + " Y " + to_string(position.y) + " Z " + to_string(position.z);
	};

	Listeners();
}

void ModuleCamera::Update()
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	auto cam = Entity->Get<StorageCamera>(camera);
	auto wnd = Global->Get<RenderWindow>("window");

	delta = clock.restart().asSeconds();

	if(!cam->Active) return;

	// rotate camera head
	Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
	Vector2i position = Mouse::getPosition(*wnd);
	if(position != center && focus)
	{
		Mouse::setPosition(center, *wnd);
		Vector2i offset = position - center;
		Rotate(vec3(offset.y, -offset.x, 0));
	}

	// move capsule body
	vec3 move;
	if (Keyboard::isKeyPressed(Keyboard::Up      ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
	if (Keyboard::isKeyPressed(Keyboard::Down    ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
	if (Keyboard::isKeyPressed(Keyboard::Left    ) || Keyboard::isKeyPressed(Keyboard::A)) move.z++;
	if (Keyboard::isKeyPressed(Keyboard::Right   ) || Keyboard::isKeyPressed(Keyboard::D)) move.z--;
	if (Keyboard::isKeyPressed(Keyboard::PageUp  ) || Keyboard::isKeyPressed(Keyboard::Q)) move.y++;
	if (Keyboard::isKeyPressed(Keyboard::PageDown) || Keyboard::isKeyPressed(Keyboard::E)) move.y--;
	if(move.length() > 0)
	{
		Keyboard::isKeyPressed(Keyboard::LShift) ? Move(move, 100.0f) : Move(move);
	}

	// synchronize camera head and capsule body
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(cam->Person);
	tsfcam->Position(tsfpsn->Position() + vec3(0, Entity->Get<StoragePerson>(cam->Person)->Eyes, 0));
	tsfpsn->Rotation(vec3(0, tsfcam->Rotation().y, 0));

	Calculate();
}

void ModuleCamera::Listeners()
{
	Event->Listen("InputBindCamera", [=]{
		auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));
		State(!cam->Active);
	});

	Event->Listen("WindowRecreated", [=]{
		State();
		Projection();
	});

	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		State();
		Projection(Size);
	});

	Event->Listen("WindowFocusLost", [=]{
		auto wnd = Global->Get<RenderWindow>("window");

		// free mouse cursor
		wnd->setMouseCursorVisible(true);
		focus = false;
	});

	Event->Listen("WindowFocusGained", [=]{
		auto wnd = Global->Get<RenderWindow>("window");
		auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));

		// recover state
		if(cam->Active)
		{
			wnd->setMouseCursorVisible(false);
			Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
			Mouse::setPosition(center, *wnd);
		}
		focus = true;
	});

	Event->Listen("InputBindJump", [=] {
		// test if body has contact to anything
		auto tsf = Entity->Get<StorageTransform>(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person);
		tsf->Body->applyCentralImpulse(btVector3(0.0f, !Keyboard::isKeyPressed(Keyboard::LShift) ? 350.0f : 1000.0f, 0.0f));
	});
}
