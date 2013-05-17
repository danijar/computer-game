#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;
using namespace sf;

#include "settings.h"
#include "form.h"
#include "camera.h"
#include "person.h"
#include "print.h"


void ModuleCamera::Init()
{
	unsigned int id = Create(vec3(0), 1.80f);
	*Global->Add<unsigned int>("camera") = id;
	Entity->Get<Camera>(id)->Active = !Global->Get<Settings>("settings")->Mouse;
	auto wnd = Global->Get<RenderWindow>("window");

	focus = Focus();
	wnd->setMouseCursorVisible(!focus);

	campitch = 0.0f;

	Calculate();
	State();
	Projection();

	Entity->Add<Print>(Entity->New())->Text = [=]{
		ivec3 position = (ivec3)Entity->Get<Form>(Entity->Get<Camera>(*Global->Get<unsigned int>("camera"))->Person)->Position();
		return "X " + to_string(position.x) + " Y " + to_string(position.y) + " Z " + to_string(position.z);
	};

	Listeners();
	Callbacks();
}

void ModuleCamera::Update()
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	auto cam = Entity->Get<Camera>(camera);
	if(!cam->Active) return;

	auto psn = Entity->Get<Person>(cam->Person);
	auto wnd = Global->Get<RenderWindow>("window");
	auto tsfcam = Entity->Get<Form>(camera);
	auto tsfpsn = Entity->Get<Form>(cam->Person);
	
	// rotate camera head
	Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
	Vector2i position = Mouse::getPosition(*wnd);
	if(position != center && focus)
	{
		Mouse::setPosition(center, *wnd);
		Vector2i offset = position - center;
		Rotate(vec3(offset.y, -offset.x, 0));
	}
	
	// synchronize camera head and capsule body
	tsfcam->Position(tsfpsn->Position() + vec3(0, Entity->Get<Person>(cam->Person)->Eyes, 0));
	tsfpsn->Rotation(vec3(0, tsfcam->Rotation().y, 0)); // this doesn't work yet

	Calculate();
}

void ModuleCamera::Listeners()
{
	Event->Listen("InputBindCamera", [=]{
		auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
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
		auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));

		// recover state
		if(cam->Active)
		{
			wnd->setMouseCursorVisible(false);
			Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
			Mouse::setPosition(center, *wnd);
		}
		focus = true;
	});
}
