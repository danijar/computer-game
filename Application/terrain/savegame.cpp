#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <ZIP/zipint.h>
using namespace std;
using namespace glm;

bool ModuleTerrain::Load(string File, Terrain *Terrain)
{
	ivec3 chunk = Terrain->Key;

	return false;
}

bool ModuleTerrain::Save(string File, Terrain *Terrain)
{
	zip *archive = zip_open((Name() + "/save/" + File +".zip").c_str(), ZIP_CREATE, 0);
 
	string name = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	struct zip_source *source = zip_source_buffer(archive, Terrain->Blocks, sizeof Terrain->Blocks, 0);
	if(source == NULL)
	{
		zip_source_free(source);
		Debug->Fail("could not store chunk to disk " + string(zip_strerror(archive)));
		return false;
	}

	zip_int64_t index =  zip_file_add(archive, name.c_str(), source, ZIP_FL_OVERWRITE);
	if(index < 0)
	{
		Debug->Fail("could not store chunk to disk " + string(zip_strerror(archive)));
		return false;
	}

	zip_close(archive);

	return true;
}
