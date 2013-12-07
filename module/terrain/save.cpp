#include "module.h"

#include <string>
#include <filesystem>
#include "dependency/glm/glm.hpp"
#include <zip/zipint.h>
using namespace std;
using namespace std::tr2::sys;
using namespace glm;

#include "type/settings/type.h"


bool ModuleTerrain::Load(Terrain *Terrain)
{
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	return Archive->Read(path, file, Terrain->Blocks);
}

void ModuleTerrain::Save(Terrain *Terrain)
{
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	Archive->WriteAsync(path, file, Terrain->Blocks, sizeof Terrain->Blocks, [=](bool result){
		if(!result) Log->Fail("save chunk failed");
	});
}
