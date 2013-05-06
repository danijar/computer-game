#include "module.h"

#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "settings.h"
#include "camera.h"
#include "transform.h"


void ModuleCamera::Projection()
{
	Projection(Global->Get<RenderWindow>("window")->getSize());
}

void ModuleCamera::Projection(Vector2u Size)
{
	auto stg = Global->Get<StorageSettings>("settings");
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));

	cam->Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 0.1f, stg->Viewdistance);
}

void ModuleCamera::Calculate()
{
	unsigned int id = *Global->Get<unsigned int>("camera");
	auto tsf = Entity->Get<StorageTransform>(id);
	auto cam = Entity->Get<StorageCamera>(id);

	cam->View = lookAt(tsf->Position(), tsf->Position() + tsf->Direction(), vec3(0, 1, 0));
}
