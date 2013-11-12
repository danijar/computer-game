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

	materials.insert(make_pair(Path, make_pair(material, Hash("module/" + Name() + "/material/" + Path))));
	return material;
}

void ModuleModel::ReloadMaterials()
{
	auto mds = Entity->Get<Model>();

	for(auto i = materials.begin(); i != materials.end(); ++i)
	{
		int hash = Hash("module/" + Name() + "/material/" + i->first);
		if(i->second.second != hash)
		{
			i->second.second = hash;

			Material previous(i->second.first);
			LoadMaterial(i->second.first, i->first);

			if(i->second.first.Diffuse != previous.Diffuse)
			{
				GLuint diffuse = GetTexture(i->second.first.Diffuse);
				for(auto j = mds.begin(); j != mds.end(); ++j)
					if(j->second->Material == i->first)
						j->second->Diffuse = diffuse;
			}

			Debug->Pass("material (" + i->first + ") reloaded");
		}
	}
}

void ModuleModel::LoadMaterial(Material &Material, string Path)
{
	ifstream stream("module/" + Name() + "/material/" + Path);
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
