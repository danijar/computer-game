#include "module.h"

#include <GLM/glm.hpp>
using namespace glm;

#include "form.h"
#include "light.h"


void ModuleSky::Init()
{
	unsigned int id = CreateSky();
	*Global->Add<unsigned int>("sky") = id;

	auto tsf = Entity->Get<Form>(id);
	tsf->Position(vec3(10, 12, 10));
}

void ModuleSky::Update()
{
	vec3 position = Entity->Get<Form>(*Global->Get<unsigned int>("camera"))->Position();
	Entity->Get<Form>(*Global->Get<unsigned int>("sky"))->Position(position);
}
