#include "module.h"

#include <string>
#include "dependency/sfml/Window.hpp"
using namespace std;
using namespace sf;

#include "type/settings/type.h"
#include "type/print/type.h"


void ModuleSettings::Init()
{
	auto stg = Global->Add<Settings>("settings");

	frames = 0;
	*Global->Add<int>("FPS") = 0;
	Entity->Add<Print>(Entity->New())->Text = [=]{
		return "FPS " + to_string(*Global->Get<int>("FPS"));
	};

	Listeners();
	Callbacks();

	Script->Run("init.js");
}

void ModuleSettings::Update()
{
	if(clock.getElapsedTime().asMilliseconds() >= 1000)
	{
		clock.restart();
		*Global->Get<int>("FPS") = frames;
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
