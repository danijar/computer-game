#include "module.h"

#include <string>
#include <filesystem>
#include <GLM/glm.hpp>
#include <ZIP/zipint.h>
using namespace std;
using namespace std::tr2::sys;
using namespace glm;

#include "settings.h"
#include "person.h"
#include "camera.h"


bool ModulePerson::Load(unsigned int Id)
{
	auto tsf = Entity->Get<Form>(Id);
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + "/data.zip";
	string file = "player.txt";

	float data[6] = { 0 };

	bool result = Archive->Read(path, file, data);

	Debug->Print("loaded x coordinate " + to_string((int)data[0]));

	// later on save position and rotation in physics component
	tsf->Position(vec3(data[0], data[1], data[2]));
	tsf->Rotation(vec3(data[3], data[4], data[5]));

	return result;
}

void ModulePerson::Save(unsigned int Id)
{
	auto tsf = Entity->Get<Form>(Id);
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + "/data.zip";
	string file = "player.txt";

	vec3 position = tsf->Position(), rotation = tsf->Rotation();
	float data[6] = { position.x, position.y, position.z, rotation.x, rotation.y, rotation.z };
	
	bool result = Archive->Write(path, file, data, 6);
	if(!result) Debug->Fail("save person failed");
	else Debug->Print("stored x coordinate " + to_string((int)data[0]));
}
