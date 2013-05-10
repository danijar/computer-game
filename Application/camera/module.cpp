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

	focus = true; // how to find out whether window was opened in background?
	campitch = 0.0f;
	onground = false;

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
	if(!cam->Active) return;

	auto psn = Entity->Get<StoragePerson>(cam->Person);
	auto wnd = Global->Get<RenderWindow>("window");
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(cam->Person);

	delta = clock.restart().asSeconds();
	
	// rotate camera head
	Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
	Vector2i position = Mouse::getPosition(*wnd);
	if(position != center && focus)
	{
		Mouse::setPosition(center, *wnd);
		Vector2i offset = position - center;
		Rotate(vec3(offset.y, -offset.x, 0));
	}

	// whether stay on ground or not
	btVector3 origin = tsfpsn->Body->getWorldTransform().getOrigin();
	onground = RayDown(origin, psn->Height / 2).first;
	const int samples = 8;
	for (int i = 0; i < samples && !onground; ++i)
	{
		float value = glm::pi<float>() * 2 * (float)i / samples;
		btVector3 direction = btVector3(sin(value), 0, cos(value)).normalize();
		onground = RayDown(origin + direction * psn->Radius, psn->Height / 2 + 0.1f).first;
	}

	// move capsule body
	vec3 move;
	if (Keyboard::isKeyPressed(Keyboard::Up      ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
	if (Keyboard::isKeyPressed(Keyboard::Down    ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
	if (Keyboard::isKeyPressed(Keyboard::Left    ) || Keyboard::isKeyPressed(Keyboard::A)) move.z++;
	if (Keyboard::isKeyPressed(Keyboard::Right   ) || Keyboard::isKeyPressed(Keyboard::D)) move.z--;
	if(length(move) > 0 && onground)
		Keyboard::isKeyPressed(Keyboard::LShift) ? Move(move, 20.0f) : Move(move);
	
	// synchronize camera head and capsule body
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
		if(Keyboard::isKeyPressed(Keyboard::LShift))
		{
			auto tsf = Entity->Get<StorageTransform>(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person);
			tsf->Body->applyCentralImpulse(btVector3(0, 1000, 0));
		}
		else if(onground)
		{
			auto tsf = Entity->Get<StorageTransform>(Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person);
			tsf->Body->applyCentralImpulse(btVector3(0, 350, 0));
		}
	});
}
