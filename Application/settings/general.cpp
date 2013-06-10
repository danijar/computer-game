#include "module.h"

#include <string>
#include <SFML/Window.hpp>
using namespace std;
using namespace sf;

#include "settings.h"
#include "print.h"


void ModuleSettings::Init()
{
	auto stg = Global->Add<Settings>("settings");

	stg->Fullscreen    = false;
	stg->Mouse		   = false;
	stg->Size          = Vector2u(800, 600);
	stg->Title         = "Graphics Application";
	stg->Fieldofview   = 45.f;
	stg->Viewdistance  = 1000.f;
	stg->Chunkdistance = 0.1f;

	frames = 0;
	Entity->Add<Print>(Entity->New())->Text = [=]{
		return "FPS " + to_string(stg->FPS);
	};

	Listeners();
}

void ModuleSettings::Update()
{
	if(clock.getElapsedTime().asMilliseconds() >= 1000)
	{
		clock.restart();
		Global->Get<Settings>("settings")->FPS = frames;
		frames = 0;
	}
	frames++;
}

void ModuleSettings::Listeners()
{
	Event->Listen("InputBindWireframe", [=]{
		auto stg = Global->Get<Settings>("settings");
		stg->Wireframe = !stg->Wireframe;
	});
}
