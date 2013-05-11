#include "module.h"

#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "form.h"
#include "camera.h"
#include "person.h"


unsigned int ModuleCamera::Create(vec3 Position, float Height)
{
	unsigned int camera = Entity->New(), person = Entity->New();
	auto tsf = Entity->Add<Form>(camera);
	Entity->Add<Camera>(camera)->Person = person;
	Entity->Add<Form>(person)->Position(Position);
	Entity->Add<Person>(person)->Calculate(Height);
	
	tsf->Body->setActivationState(DISABLE_DEACTIVATION);
	tsf->Position(Position);

	return camera;
}
