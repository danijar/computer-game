#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/constants.hpp>
using namespace sf;
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "transform.h"
#include "text.h"


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
		unsigned int camera = *Global->Get<unsigned int>("camera");
		auto cam = Entity->Get<StorageCamera>(camera);
		auto tsfcam = Entity->Get<StorageTransform>(camera);
		auto tsfpsn = Entity->Get<StorageTransform>(cam->Person);
		ivec3 rotcam = (ivec3)degrees(tsfcam->Rotation());
		ivec3 rotpsn = (ivec3)degrees(tsfpsn->Rotation());
		return "X " + to_string((int)floor(tsfcam->Position().x)) + " Y " + to_string((int)floor(tsfcam->Position().y)) + " Z " + to_string((int)floor(tsfcam->Position().z)) + '\n'
			 + "cam Yaw " + to_string(rotcam.y) + " Pitch " + to_string(rotcam.x) + " Roll " + to_string(rotcam.z) + "\n"
			 + "psn Yaw " + to_string(rotpsn.y) + " Pitch " + to_string(rotpsn.x) + " Roll " + to_string(rotpsn.z);
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

	// synchronization
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(person);
	tsfcam->Position(tsfpsn->Position() + vec3(0, 1.8f /* meters to eyes */, 0));
	//tsfpsn->Rotation(vec3(0, tsfcam->Rotation().y, 0));

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
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);

	//tsf->Rotation(tsf->Rotation() + vec3(-Amount.y * Speed, -Amount.x * Speed, 0));
	btTransform transform = tsf->Body->getWorldTransform();
	btQuaternion rotation = transform.getRotation();
	rotation = rotation * btQuaternion(-Amount.x * Speed, -Amount.y * Speed, 0);
	transform.setRotation(rotation);
	tsf->Body->setWorldTransform(transform);
}

void ModuleCamera::Move(vec3 Amount, float Speed)
{
	unsigned int camera = *Global->Get<unsigned int>("camera");
	unsigned int person = Entity->Get<StorageCamera>(camera)->Person;
	auto tsfcam = Entity->Get<StorageTransform>(camera);
	auto tsfpsn = Entity->Get<StorageTransform>(person);

	vec3 forward(sinf(tsfcam->Rotation().y), 0, cosf(tsfcam->Rotation().y));
	vec3 right(-forward.z, 0, forward.x);
	vec3 up(0, 1, 0);

	vec3 combined(Amount.x * forward + Amount.y * up + Amount.z * right);
	tsfpsn->Body->setLinearVelocity(btVector3(combined.x, combined.y, combined.z) * Speed);
}

void ModuleCamera::Calculate()
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);
	auto cam = Entity->Get<StorageCamera>(id);

	/*
	 * doesn't work with physics simulation's wrapping
	 * const float margin = 0.2f;
	 * const float pi = glm::pi<float>();
	 * if     (tsf->Rotation().x < -pi/2 + margin) { vec3 rotation = tsf->Rotation(); tsf->Rotation(vec3(-pi/2 + margin, rotation.y, rotation.z)); }
	 * else if(tsf->Rotation().x >  pi/2 - margin) { vec3 rotation = tsf->Rotation(); tsf->Rotation(vec3( pi/2 - margin, rotation.y, rotation.z)); }
	 */

	//vec3 lookat(sinf(tsf->Rotation().y) * cosf(tsf->Rotation().x), sinf(tsf->Rotation().x), cosf(tsf->Rotation().y) * cosf(tsf->Rotation().x));
	btVector3 axis = tsf->Body->getWorldTransform().getRotation().getAxis();
	vec3 lookat(axis.getX(), axis.getY(), axis.getZ());

	cam->View = lookAt(tsf->Position(), tsf->Position() + normalize(lookat), vec3(0, 1, 0));
}
