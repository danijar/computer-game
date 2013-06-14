#include "module.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;

#include "model.h"


ModuleModel::Material ModuleModel::GetMaterial(string Path)
{
	auto i = materials.find(Path);
	if(i != materials.end()) return i->second.first;

	Material material;
	LoadMaterial(material, Path);

	materials.insert(make_pair(Path, make_pair(material, Hash(Name() + "/material/" + Path))));
	return material;
}

void ModuleModel::ReloadMaterials()
{
	auto mds = Entity->Get<Model>();

	for(auto i = materials.begin(); i != materials.end(); ++i)
	{
		int hash = Hash(Name() + "/material/" + i->first);
		if(i->second.second != hash)
		{
			auto mtl = i->second.first;
			pair<string, GLuint> previous = make_pair(mtl.Diffuse, GetTexture(mtl.Diffuse));

			i->second.second = hash;
			LoadMaterial(mtl, i->first);
			Debug->Pass("material (" + i->first + ") reloaded");

			if(mtl.Diffuse != previous.first)
			{
				auto fresh = make_pair(mtl.Diffuse, GetTexture(mtl.Diffuse));
				for(auto j = mds.begin(); j != mds.end(); ++j)
					if(j->second->Diffuse == previous.second)
						j->second->Diffuse = fresh.second;
			}
			
		}
	}
}

void ModuleModel::LoadMaterial(Material &Material, string Path)
{
	ifstream stream(Name() + "/material/" + Path);
	if(!stream.is_open())
	{
		Debug->Fail("material (" + Path + ") cannot be loaded");
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
