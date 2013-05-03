#include "module.h"

#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/euler_angles.hpp>
using namespace glm;

#include "transform.h"


void ModulePhysic::Matrix(unsigned int id)
{
	auto tsf = Entity->Get<StorageTransform>(id);

	mat4 Scale     = scale    (mat4(1), tsf->Scale);
	mat4 Translate = translate(mat4(1), tsf->Position);
	vec3 rotation  = radians(tsf->Rotation);
	mat4 Rotate    = eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
	tsf->Matrix = Translate * Rotate * Scale;
}
