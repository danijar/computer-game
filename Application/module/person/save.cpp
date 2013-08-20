#include "module.h"

#include <string>
#include <GLM/glm.hpp>
using namespace std;
using namespace glm;

#include "settings.h"
#include "person.h"
#include "camera.h"


bool ModulePerson::Load(unsigned int Id)
{
	auto stg = Global->Get<Settings>("settings");
	auto cam = Entity->Get<Camera>(Id);
	auto tsfcam = Entity->Get<Form>(Id);
	auto tsfpsn = Entity->Get<Form>(cam->Person);

	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + "/data.zip";
	string file = "player.txt";

	float data[8] = { 0 };

	bool result = Archive->Read(path, file, data);

	if(result)
	{
		cam->Pitch = data[0];
		tsfpsn->Position(vec3(data[1], data[2], data[3]));
		tsfcam->Quaternion(quat(data[4], data[5], data[6], data[7]));
	}
	else
	{
		Debug->Fail("load person fail");
	}

	return result;
}

void ModulePerson::Save(unsigned int Id)
{
	auto stg = Global->Get<Settings>("settings");
	auto cam = Entity->Get<Camera>(Id);
	auto tsfcam = Entity->Get<Form>(Id);
	auto tsfpsn = Entity->Get<Form>(cam->Person);
	
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + "/data.zip";
	string file = "player.txt";

	// later on save position and rotation in physics module
	// and campitch in camera module

	vec3 position = tsfpsn->Position();
	quat rotation = tsfcam->Quaternion();
	float data[8] = { cam->Pitch, position.x, position.y, position.z, rotation.w, rotation.x, rotation.y, rotation.z };
	
	/*
	 * why doesn't this work asynchronously?
	 *
	 * Archive->WriteAsync(path, file, data, 6, [=](bool result){
	 *     if(!result) Debug->Fail("save person failed");
	 * });
	 */

	bool result = Archive->Write(path, file, data, sizeof data);
	if(!result) Debug->Fail("save person fail");
}
