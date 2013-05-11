#include "module.h"

#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "camera.h"
#include "person.h"


unsigned int ModuleCamera::Create(vec3 Position, float Height)
{
	unsigned int camera = Entity->New(), person = Entity->New();
	auto tsf = Entity->Add<StorageTransform>(camera);
	Entity->Add<StorageCamera>(camera)->Person = person;
	Entity->Add<StorageTransform>(person)->Position(Position);
	Entity->Add<StoragePerson>(person)->Calculate(Height);
	
	tsf->Body->setActivationState(DISABLE_DEACTIVATION);
	tsf->Position(Position);

	return camera;
}
