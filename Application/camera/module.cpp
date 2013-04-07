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


void ModuleCamera::Init()
{
	auto stg = Global->Get<StorageSettings>("settings");
	unsigned int id = Entity->New();
	auto cam = Entity->Add<StorageCamera>(id);
	auto tsf = Entity->Add<StorageTransform>(id);
	cam->Active = !stg->Mouse;
	tsf->Position = vec3(0, 5, -5);
	*Global->Add<unsigned int>("camera") = id;

	Calculate();
	State();
	Projection();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{
		return "X " + to_string((int)floor(tsf->Position.x)) + " Y " + to_string((int)floor(tsf->Position.y)) + " Z " + to_string((int)floor(tsf->Position.z));
	};

	Listeners();
}

void ModuleCamera::Update()
{
	auto wnd = Global->Get<RenderWindow>("window");
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));
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

void ModuleCamera::Rotate(Vector2i Amount)
{
	auto tsf = Entity->Get<StorageTransform>(*Global->Get<unsigned int>("camera"));

	const float speed = .08f;
	tsf->Rotation += vec3(vec2(-Amount.x, -Amount.y) * speed * delta, 0);
}

void ModuleCamera::Move(vec3 Amount, float Speed)
{
	auto tsf = Entity->Get<StorageTransform>(*Global->Get<unsigned int>("camera"));

	vec3 forward = vec3(sinf(tsf->Rotation.x), 0, cosf(tsf->Rotation.x));
	vec3 right = vec3(-forward.z, 0, forward.x);

	tsf->Position   += forward * Amount.x * Speed * delta;
	tsf->Position.y +=           Amount.y * Speed * delta;
	tsf->Position   += right   * Amount.z * Speed * delta;
}

void ModuleCamera::Calculate()
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);
	auto cam = Entity->Get<StorageCamera>(id);

	const float pi = glm::pi<float>();
	if		(tsf->Rotation.x < -pi)	tsf->Rotation.x += pi*2;
	else if	(tsf->Rotation.x >  pi)	tsf->Rotation.x -= pi*2;
 
	const float margin = 0.2f;
	if		(tsf->Rotation.y < -pi/2+margin) tsf->Rotation.y = -pi/2+margin;
	else if	(tsf->Rotation.y >  pi/2-margin) tsf->Rotation.y =  pi/2-margin;

	vec3 lookat(
		sinf(tsf->Rotation.x) * cosf(tsf->Rotation.y),
			                    sinf(tsf->Rotation.y),
		cosf(tsf->Rotation.x) * cosf(tsf->Rotation.y)
	);

	cam->View = lookAt(tsf->Position, tsf->Position + lookat, vec3(0, 1, 0));
}
