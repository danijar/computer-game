#include "module.h"

#include <string>
#include <glm/gtc/noise.hpp>
using namespace std;
using namespace glm;

#include "type/model/type.h"


bool ModuleRenderer::Visible(uint64_t Id)
{
	auto mdl = Entity->Get<Model>(Id);
	if (!mdl->Box.X.Min && !mdl->Box.X.Max && !mdl->Box.Y.Min && !mdl->Box.Y.Max && !mdl->Box.Z.Min && !mdl->Box.Z.Max) return true;


	return true;
}
