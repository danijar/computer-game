#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;


class Materials
{
public:
	struct Data
	{
		string Name;
		string Diffuse, Normal, Specular;
	};

	Data Get(string Path)
	{
		auto i = list.find(Path);
		if(i != list.end()) return i->second;

		Data material;
		Load(material, Path);

		list.insert(make_pair(Path, material));
		return material;
	}

	void Reload()
	{
		for(auto i = list.begin(); i != list.end(); ++i)
		{
			// check if the file actually changed
			Load(i->second, i->first);
		}
	}

private:
	void Load(Data &Material, string Path)
	{
		Data material;
		ifstream stream("model/material/" + Path);
		if(!stream.is_open())
		{
			// Debug->Fail("(" + Path + ") cannot be loaded.");
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

	unordered_map<string, Data> list;
};
