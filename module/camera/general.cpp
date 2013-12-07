#include "module.h"

#include "dependency/sfml/Window.hpp"
#include "dependency/sfml/Graphics/RenderWindow.hpp"
#include "dependency/glm/glm.hpp"
using namespace glm;
using namespace sf;

#include "type/settings/type.h"
#include "type/form/type.h"
#include "type/camera/type.h"
#include "type/person/type.h"


void ModuleCamera::Init()
{
	// load cameras
	Load();

	// if there is no camera at all, add a default one
	if(Entity->Get<Camera>().size() < 1)
	{
		uint64_t id = Entity->New();
		auto cam = Entity->Add<Camera>(id);
		auto frm = Entity->Add<Form>(id);

		frm->Body->setActivationState(DISABLE_DEACTIVATION);
		frm->Position(vec3(0, 10, 0));

		Log->Pass("none loaded, added default one");
	}
	
	// if there is no bound camera, bound the first one
	if(!Global->Check("camera"))
	{
		uint64_t id = Entity->Get<Camera>().begin()->first;
		Entity->Get<Camera>(id)->Active = !*Global->Get<Settings>("settings")->Get<bool>("Mouse");
		*Global->Add<uint64_t>("camera") = id;
		Log->Pass("not bound, fallback to first one");
	}

	// initially calculate perspektive
	// and set window and mouse state
	Projection();
	State(Focus());

	Listeners();
	Callbacks();
}

void ModuleCamera::Update()
{
	uint64_t camera = *Global->Get<uint64_t>("camera");
	auto cam = Entity->Get<Camera>(camera);
	auto wnd = Global->Get<RenderWindow>("window");

	// rotate camera head
	Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
	Vector2i position = Mouse::getPosition(*wnd);
	if(cam->Active && position != center)
	{
		Mouse::setPosition(center, *wnd);
		Vector2i offset = position - center;
		Rotate(vec3(offset.y, -offset.x, 0));
	}

	// synchronize camera head and capsule body
	if(cam->Person)
	{
		auto frmcam = Entity->Get<Form>(camera);
		auto frmpsn = Entity->Get<Form>(cam->Person);
		frmcam->Position(frmpsn->Position() + vec3(0, Entity->Get<Person>(cam->Person)->Eyes, 0));
		frmpsn->Rotation(vec3(0, Yaw(), 0));
	}

	// store cameras every some milliseconds
	static Clock clock;
	if(clock.getElapsedTime().asMilliseconds() > 500)
	{
		Save();
		clock.restart();
	}

	Calculate();
}

void ModuleCamera::Listeners()
{
	Event->Listen("InputBindCamera", [=]{
		auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));
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
		Global->Get<RenderWindow>("window")->setMouseCursorVisible(true);
	});

	Event->Listen("WindowFocusGained", [=]{
		auto wnd = Global->Get<RenderWindow>("window");
		auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));

		// recover state
		if(cam->Active)
		{
			wnd->setMouseCursorVisible(false);
			Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
			Mouse::setPosition(center, *wnd);
		}
	});
}
