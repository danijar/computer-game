#include "module.h"

#include <SFML/Window.hpp>
using namespace sf;

#include "settings.h"


void ModuleSettings::Init()
{
	auto stg = Global->Add<StorageSettings>("settings");

	stg->Fullscreen   = false;
	stg->Mouse		  = false;
	stg->Size         = Vector2u(800, 600);
	stg->Title        = "Graphics Application";
	stg->Fieldofview  = 45.f;
	stg->Viewdistance = 1000.f;

	Listeners();
}

void ModuleSettings::Update()
{

}

void ModuleSettings::Listeners()
{
	Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code) {
		auto stg = Global->Get<StorageSettings>("settings");
		switch(Code)
		{
		case Keyboard::Key::F3:
			stg->Wireframe = !stg->Wireframe;
			break;
		}
	});
}
