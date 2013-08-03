#include "module.h"

#include <string>
#include <filesystem>
#include <GLM/glm.hpp>
#include <ZIP/zipint.h>
using namespace std;
using namespace std::tr2::sys;
using namespace glm;

#include <settings.h>


bool ModuleTerrain::Load(string File, Terrain *Terrain)
{
	string name = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	zip *archive = zip_open(("module/" + Name() + "/save/" + File +".zip").c_str(), 0, NULL);
	if(archive == NULL)
		return false;

	struct zip_stat stat;
	zip_stat_init(&stat);
	int result = zip_stat(archive, name.c_str(), 0, &stat);
	if(result < 0)
	{
		zip_close(archive);
		return false;
	}

	zip_file *file = zip_fopen(archive, name.c_str(), 0);
	if(file == NULL)
	{
		zip_fclose(file);
		zip_close(archive);
		return false;
	}

	zip_fread(file, Terrain->Blocks, stat.size);
	zip_fclose(file);

	zip_close(archive);

	return true;
}

bool ModuleTerrain::Save(string File, Terrain *Terrain)
{
	path dir("module/" + Name() + "/save");
	if(!exists(dir)) create_directory(dir);

	string name = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	zip *archive = zip_open(("module/" + Name() + "/save/" + File +".zip").c_str(), ZIP_CREATE, NULL);
	if(archive == NULL)
		return false;

	struct zip_source *source = zip_source_buffer(archive, Terrain->Blocks, sizeof Terrain->Blocks, 0);
	if(source == NULL)
	{
		zip_source_free(source);
		zip_close(archive);
		return false;
	}

	zip_int64_t index = zip_file_add(archive, name.c_str(), source, ZIP_FL_OVERWRITE);
	if(index < 0)
	{
		zip_close(archive);
		return false;
	}

	int result = zip_close(archive);
	if(result < 0)
	{
		Debug->Fail("zip_close: " + string(zip_strerror(archive)));
		return false;
	}

	return true;
}

void ModuleTerrain::World(string Name)
{
	while(loading);
	null = true;

	auto tns = Entity->Get<Terrain>();

	for(auto i : tns)
	{
		auto mdl = Entity->Get<Model>(i.first);
		glDeleteBuffers(1, &mdl->Positions);
		glDeleteBuffers(1, &mdl->Normals);
		glDeleteBuffers(1, &mdl->Texcoords);
		glDeleteBuffers(1, &mdl->Elements);

		Entity->Delete(i.first);
	}

	world = Name;
	Global->Get<Settings>("settings")->Set<string>("World", Name);
}
