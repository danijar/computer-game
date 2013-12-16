#include "module.h"

#include <string>
#include <dependency/glm/glm.hpp>
using namespace std;
using namespace glm;

#include "type/settings/type.h"
#include "type/person/type.h"


void ModulePerson::Load()
{
	auto stg = Global->Get<Settings>("settings");

	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = "data.txt";
	
	// for now, only load the first camera
	float data[7] = { 0 };
	if(Archive->Read(path, file, data))
	{
		uint64_t id = Entity->New();
		auto psn = Entity->Add<Person>(id);
		auto frm = Entity->Add<Form>(id);

		frm->Position(vec3(data[0], data[1], data[2]));
		frm->Quaternion(quat(data[3], data[4], data[5], data[6]));
		psn->Calculate(1.80f);
	}
}

void ModulePerson::Save()
{
	auto stg = Global->Get<Settings>("settings");
	auto pns = Entity->Get<Person>();
	if(pns.size() < 1) return;

	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = "data.txt";

	// for now, only save the first person
	uint64_t id = pns.begin()->first;
	auto psn = Entity->Get<Person>(id);
	auto frm = Entity->Get<Form>(id);
	
	vec3 position = frm->Position();
	quat rotation = frm->Quaternion();
	float data[7] = { position.x, position.y, position.z, rotation.w, rotation.x, rotation.y, rotation.z };

	/*
	 * why doesn't this work asynchronously?
	 *
	 * Archive->WriteAsync(path, file, data, 6, [=](bool result){
	 *     if(!result) Log->Fail("save person failed");
	 * });
	 */

	bool result = Archive->Write(path, file, data, sizeof data);
	if(!result) Log->Fail("save fail");
}
