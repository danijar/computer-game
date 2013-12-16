#include "module.h"

#include <dependency/glm/glm.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>

#include "type/settings/type.h"
#include "type/camera/type.h"
#include "type/form/type.h"

using namespace glm;
using namespace sf;


void ModuleCamera::Projection()
{
	Projection(Global->Get<RenderWindow>("window")->getSize());
}

void ModuleCamera::Projection(Vector2u Size)
{
	auto stg = Global->Get<Settings>("settings");
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));

	cam->Projection = perspective(*stg->Get<float>("Fieldofview"), (float)Size.x / (float)Size.y, 0.1f, *stg->Get<float>("Viewdistance"));
}

void ModuleCamera::Calculate()
{
	uint64_t id = *Global->Get<uint64_t>("camera");
	auto frm = Entity->Get<Form>(id);
	auto cam = Entity->Get<Camera>(id);

	cam->View = lookAt(frm->Position(), frm->Position() + frm->Direction(), vec3(0, 1, 0));
}
