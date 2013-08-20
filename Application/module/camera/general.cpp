#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/glm.hpp>
using namespace glm;
using namespace sf;

#include "settings.h"
#include "form.h"
#include "camera.h"
#include "person.h"


void ModuleCamera::Init()
{
	unsigned int id = Entity->New();
	Entity->Add<Camera>(id)->Active = !*Global->Get<Settings>("settings")->Get<bool>("Mouse");
	Entity->Add<Form>(id)->Body->setActivationState(DISABLE_DEACTIVATION);
	*Global->Add<unsigned int>("camera") = id;
	auto wnd = Global->Get<RenderWindow>("window");

	focus = Focus();
	wnd->setMouseCursorVisible(!focus);

	Calculate();
	State();
	Projection();

	Listeners();
	Callbacks();
}

void ModuleCamera::Update()
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	auto cam = Entity->Get<Camera>(camera);
	if(!cam->Active) return;

	auto wnd = Global->Get<RenderWindow>("window");

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
	if(cam->Person)
	{
		auto tsfcam = Entity->Get<Form>(camera);
		auto tsfpsn = Entity->Get<Form>(cam->Person);
		tsfcam->Position(tsfpsn->Position() + vec3(0, Entity->Get<Person>(cam->Person)->Eyes, 0));
		tsfpsn->Rotation(vec3(0, Yaw(), 0));
	}

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
