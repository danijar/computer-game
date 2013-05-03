#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/constants.hpp>
using namespace sf;
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "transform.h"
#include "text.h"
#include "physic.h"


void ModuleCamera::Init()
{
	auto stg = Global->Get<StorageSettings>("settings");
	unsigned int id = Personrcamera(vec3(0, 5, -5));
	Entity->Get<StorageCamera>(id)->Active = !stg->Mouse;
	*Global->Add<unsigned int>("camera") = id;

	focus = true; // how to find out whether window was opened in background?

	Calculate();
	State();
	Projection();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{
		//auto tsf = Entity->Get<StorageTransform>(*Global->Get<unsigned int>("camera"));
		//return "X " + to_string((int)floor(tsf->Position.x)) + " Y " + to_string((int)floor(tsf->Position.y)) + " Z " + to_string((int)floor(tsf->Position.z));
		unsigned int camera = *Global->Get<unsigned int>("camera");
		auto cam = Entity->Get<StorageCamera>(camera);
		auto tsfcam = Entity->Get<StorageTransform>(camera);
		auto tsfpsn = Entity->Get<StorageTransform>(cam->Person);
		return "camera X " + to_string((int)floor(tsfcam->Position.x)) + " Y " + to_string((int)floor(tsfcam->Position.y)) + " Z " + to_string((int)floor(tsfcam->Position.z)) + " Yaw (Y) " + to_string((int)floor(tsfcam->Rotation.y)) + " Pitch (X) " + to_string((int)floor(tsfcam->Rotation.x)) + " Roll (Z) " + to_string((int)floor(tsfcam->Rotation.z)) + "\n"
			 + "person X " + to_string((int)floor(tsfpsn->Position.x)) + " Y " + to_string((int)floor(tsfpsn->Position.y)) + " Z " + to_string((int)floor(tsfpsn->Position.z)) + " Yaw (Y) " + to_string((int)floor(tsfpsn->Rotation.y)) + " Pitch (X) " + to_string((int)floor(tsfpsn->Rotation.x)) + " Roll (Z) " + to_string((int)floor(tsfpsn->Rotation.z));
	};

	Listeners();
}

void ModuleCamera::Update()
{
	auto wnd = Global->Get<RenderWindow>("window");
	unsigned int camera = *Global->Get<unsigned int>("camera");
	auto cam = Entity->Get<StorageCamera>(camera);
	unsigned int person = cam->Person;

	delta = clock.restart().asSeconds();

	if(!cam->Active) return;

	// synchronization
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(person);
	tsfcam->Position = tsfpsn->Position + vec3(0, 1.8f /* meters to eyes */, 0);
	tsfcam->Rotation.y = tsfpsn->Rotation.y;
	tsfcam->Changed = true;
	
	// rotation
	Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
	Vector2i position = Mouse::getPosition(*wnd);
	if(position != center && focus)
	{
		Mouse::setPosition(center, *wnd);
		Vector2i offset = position - center;
		Rotate(offset);
	}

	// position
	vec3 move;
	if (Keyboard::isKeyPressed(Keyboard::Up      ) || Keyboard::isKeyPressed(Keyboard::W)) move.x++;
	if (Keyboard::isKeyPressed(Keyboard::Down    ) || Keyboard::isKeyPressed(Keyboard::S)) move.x--;
	if (Keyboard::isKeyPressed(Keyboard::Right   ) || Keyboard::isKeyPressed(Keyboard::D)) move.z++;
	if (Keyboard::isKeyPressed(Keyboard::Left    ) || Keyboard::isKeyPressed(Keyboard::A)) move.z--;
	if (Keyboard::isKeyPressed(Keyboard::PageUp  ) || Keyboard::isKeyPressed(Keyboard::Q)) move.y++;
	if (Keyboard::isKeyPressed(Keyboard::PageDown) || Keyboard::isKeyPressed(Keyboard::E)) move.y--;
	Move(move, 10.f * (Keyboard::isKeyPressed(Keyboard::LShift) ? 10 : 1));

	Calculate();
}

void ModuleCamera::Listeners()
{
	Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
		auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));
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

	Event->Listen("WindowFocusLost", [=]{
		auto wnd = Global->Get<RenderWindow>("window");

		wnd->setMouseCursorVisible(true);
		focus = false;
	});

	Event->Listen("WindowFocusGained", [=]{
		auto wnd = Global->Get<RenderWindow>("window");
		auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));

		if(cam->Active)
		{
			wnd->setMouseCursorVisible(false);
			Vector2i center(wnd->getSize().x / 2, wnd->getSize().y / 2);
			Mouse::setPosition(center, *wnd);
		}
		focus = true;
	});
}

void ModuleCamera::State()
{
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));
	State(cam->Active);
}

void ModuleCamera::State(bool Active)
{
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));
	auto wnd = Global->Get<RenderWindow>("window");

	cam->Active = Active;
	wnd->setMouseCursorVisible(!Active);

	if(Active) Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
}

void ModuleCamera::Projection()
{
	Projection(Global->Get<RenderWindow>("window")->getSize());
}
void ModuleCamera::Projection(Vector2u Size)
{
	auto stg = Global->Get<StorageSettings>("settings");
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));

	cam->Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
}

void ModuleCamera::Rotate(Vector2i Amount, float Speed)
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	unsigned int person = Entity->Get<StorageCamera>(camera)->Person;
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(person);

	// use forces instead, dont't involve delta time anymore

	vec3 rotation(vec2(-Amount.y, -Amount.x) * Speed * delta, 0);

	tsfcam->Rotation += vec3(rotation.x, 0, 0);
	tsfpsn->Rotation += vec3(0, rotation.y, 0);
	tsfcam->Changed = true;
	tsfpsn->Changed = true;
}

void ModuleCamera::Move(vec3 Amount, float Speed)
{
	unsigned int person = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"))->Person;
	auto tsf = Entity->Get<StorageTransform>(person);
	auto bdy = Entity->Get<StoragePhysic>(person);

	vec3 rotation = radians(tsf->Rotation);
	vec3 forward = vec3(sinf(rotation.y), 0, cosf(rotation.y));
	vec3 right = vec3(-forward.z, 0, forward.x);

	// use forces instead, dont't involve delta time anymore
	tsf->Position   += forward * Amount.x * Speed * delta;
	tsf->Position.y +=           Amount.y * Speed * delta;
	tsf->Position   += right   * Amount.z * Speed * delta;
	tsf->Changed = true;
}

void ModuleCamera::Calculate()
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);
	auto cam = Entity->Get<StorageCamera>(id);

	const int margin = 10;
	if		(tsf->Rotation.x < -90 + margin) tsf->Rotation.x = -90 + margin;
	else if	(tsf->Rotation.x >  90 - margin) tsf->Rotation.x =  90 - margin;

	vec3 rotation = radians(tsf->Rotation);
	vec3 lookat(
		sinf(rotation.y) * cosf(rotation.x),
			               sinf(rotation.x),
		cosf(rotation.y) * cosf(rotation.x)
	);

	cam->View = lookAt(tsf->Position, tsf->Position + lookat, vec3(0, 1, 0));
}
