#include "module.h"

#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;

#include "settings.h"
#include "print.h"


void ModuleWindow::Create()
{
	auto stg = Global->Get<Settings>("settings");
	Create(!*stg->Get<bool>("Fullscreen"));
}

void ModuleWindow::Create(bool Fullscreen)
{
	auto wnd = Global->Get<RenderWindow>("window");
	auto stg = Global->Get<Settings>("settings");

	stg->Set<bool>("Fullscreen", Fullscreen);

	bool Recreated = wnd->isOpen();
	VideoMode mde = VideoMode::getDesktopMode();

	ContextSettings cts;
	cts.depthBits         = 32;
	cts.stencilBits       =  0;
	cts.antialiasingLevel =  0;
	cts.majorVersion      =  3;
	cts.minorVersion      =  3;

	if(Fullscreen)
	{
		stg->Set<>("Position", wnd->getPosition());
		wnd->create(VideoMode(mde.width, mde.height), *stg->Get<string>("Title"), Style::Fullscreen, cts);
	}
	else
	{
		wnd->create(VideoMode(stg->Get<Vector2u>("Size")->x, stg->Get<Vector2u>("Size")->y), *stg->Get<string>("Title"), Style::Default, cts);
		wnd->setPosition(*stg->Get<Vector2i>("Position"));
	}
	wnd->setVerticalSyncEnabled(*stg->Get<bool>("Verticalsync"));

	Debug->PassFail("creation", wnd->isOpen());

	if(Recreated) Event->Fire<bool>("WindowRecreated", Fullscreen);
}

void ModuleWindow::Close()
{
	Event->Fire("WindowClose");
	Global->Get<RenderWindow>("window")->close();
	Exit("The window was closed.");
}
