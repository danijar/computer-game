#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <ZIP/zipint.h>
using namespace std;
using namespace glm;

bool ModuleTerrain::Load(string File, Terrain *Terrain)
{
	string name = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	zip *archive = zip_open((Name() + "/save/" + File +".zip").c_str(), 0, NULL);
	if(archive == NULL)
		return false;

	struct zip_stat stat;
	zip_stat_init(&stat);
	int result = zip_stat(archive, name.c_str(), 0, &stat);
	if(result < 0)
		return false;

	zip_file *file = zip_fopen(archive, name.c_str(), 0);
	if(file == NULL)
		return false;

	zip_fread(file, Terrain->Blocks, stat.size);
	zip_fclose(file);

	zip_close(archive);

	return true;
}

bool ModuleTerrain::Save(string File, Terrain *Terrain)
{
	string name = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	zip *archive = zip_open((Name() + "/save/" + File +".zip").c_str(), ZIP_CREATE, NULL);
	if(archive == NULL)
	{
		Debug->Fail("could not open world file " + string(zip_strerror(archive)));
		return false;
	}

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
