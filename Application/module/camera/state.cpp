#include "module.h"

#include <sfml/Window.hpp>
#include <sfml/Graphics/RenderWindow.hpp>
using namespace sf;

#include "camera.h"


void ModuleCamera::State()
{
	auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
	State(cam->Active);
}

void ModuleCamera::State(bool Active)
{
	auto cam = Entity->Get<Camera>(*Global->Get<unsigned int>("camera"));
	auto wnd = Global->Get<RenderWindow>("window");

	cam->Active = Active;
	wnd->setMouseCursorVisible(!Active);

	if(Active) Mouse::setPosition(Vector2i(wnd->getSize().x / 2, wnd->getSize().y / 2), *wnd);
}

#ifdef _WIN32
#include <Windows.h>
bool ModuleCamera::Focus()
{
	auto wnd = Global->Get<RenderWindow>("window");

	HWND handle = wnd->getSystemHandle();
	bool focus      = (handle == GetFocus());
	bool foreground = (handle == GetForegroundWindow());

	// focus window if in foreground.
	if(focus != foreground)
	{
		SetFocus(handle);
		SetForegroundWindow(handle);
	}

	return focus;
}
#else
bool ModuleCamera::Focus()
{
	return false;
}
#endif
