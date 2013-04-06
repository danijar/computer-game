#include "module.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;


ModuleModel::Material ModuleModel::GetMaterial(string Path)
{
	auto i = materials.find(Path);
	if(i != materials.end()) return i->second;

	Material material;
	LoadMaterial(material, Path);

	materials.insert(make_pair(Path, material));
	return material;
}

void ModuleModel::ReloadMaterials()
{
	for(auto i = materials.begin(); i != materials.end(); ++i)
	{
		// check if the file actually changed
		LoadMaterial(i->second, i->first);
	}
}

void ModuleModel::LoadMaterial(Material &Material, string Path)
{
	ifstream stream("model/material/" + Path);
	if(!stream.is_open())
	{
		Debug->Fail("(" + Path + ") cannot be loaded.");
		return;
	}

	string line;
	while(getline(stream, line))
	{
		istringstream input(line);
		string key;
		input >> key;

		if(key == "") continue;
		else if(key == "newmtl"  ) input >> Material.Name;
		else if(key == "map_Kd"  ) input >> Material.Diffuse;
		else if(key == "map_Bump") input >> Material.Normal;
		else if(key == "map_Ns"  ) input >> Material.Specular;
	}
}
