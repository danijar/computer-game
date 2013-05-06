#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "camera.h"


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
