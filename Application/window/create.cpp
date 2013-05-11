#include "module.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "settings.h"
#include "text.h"


void ModuleWindow::Create()
{
	auto stg = Global->Get<StorageSettings>("settings");
	Create(!stg->Fullscreen);
}

void ModuleWindow::Create(bool Fullscreen)
{
	auto wnd = Global->Get<RenderWindow>("window");
	auto stg = Global->Get<StorageSettings>("settings");

	stg->Fullscreen = Fullscreen;

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
		stg->Position = wnd->getPosition();
		wnd->create(VideoMode(mde.width, mde.height), stg->Title, Style::Fullscreen, cts);
	}
	else
	{
		wnd->create(VideoMode(stg->Size.x, stg->Size.y), stg->Title, Style::Default, cts);
		wnd->setPosition(stg->Position);
	}
	wnd->setVerticalSyncEnabled(stg->Verticalsync);

	Debug->PassFail("creation", wnd->isOpen());

	if(Recreated) Event->Fire<bool>("WindowRecreated", Fullscreen);
}

void ModuleWindow::Close()
{
	Event->Fire("WindowClose");
	Global->Get<RenderWindow>("window")->close();
	Exit("The window was closed.");
}
