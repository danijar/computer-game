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

	// window
	stg->Set<bool>("Fullscreen", false);
	stg->Set<bool>("Mouse", false);
	stg->Set<Vector2u>("Size", Vector2u(800, 600));
	stg->Set<string>("Title", "Graphics Application");
	stg->Set<Vector2i>("Position", Vector2i(0, 0));

	// renderer
	stg->Set<float>("Fieldofview", 45.f);
	stg->Set<bool>("Wireframe", false);
	stg->Set<bool>("Verticalsync", true);
	stg->Set<float>("Viewdistance", 1000.f);

	// terrain
	stg->Set<float>("Chunkdistance", 0.1f);
	stg->Set<string>("World", "world");

	frames = 0;
	Entity->Add<Print>(Entity->New())->Text = [=]{
		return "FPS " + to_string(*stg->Get<int>("FPS"));
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
		stg->Set<bool>("Wireframe", !*stg->Get<bool>("Wireframe"));
	});
}
