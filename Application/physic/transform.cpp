#include "module.h"

#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "transform.h"


void ModulePhysic::Matrix(unsigned int id)
{
	auto tsf = Entity->Get<StorageTransform>(id);

	mat4 Scale      = scale    (mat4(1), tsf->Scale);
	mat4 Translate  = translate(mat4(1), tsf->Position);
	mat4 Rotate     = rotate   (mat4(1), tsf->Rotation.x, vec3(1, 0, 0))
					* rotate   (mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
					* rotate   (mat4(1), tsf->Rotation.z, vec3(0, 0, 1));
	tsf->Matrix = Translate * Rotate * Scale;
}
