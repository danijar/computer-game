#pragma once

#include "system.h"
#include "debug.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
#include "GLM/glm.hpp"
using namespace glm;

#include "material.h"
#include "texture.h"


class ModuleMaterial : public Module
{
	struct Material
	{
		Material() : Name(""), Diffuse(""), Normal(""), Specular("") {}
		string Name;
		string Diffuse, Normal, Specular;
		vec3 Color;
	};

	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto mts = Entity->Get<StorageMaterial>();
		for(auto i = mts.begin(); i != mts.end(); ++i)
		{
			if(i->second->Changed)
			{
				Material material = Parse(i->second->Path);

				if(material.Diffuse  != "") i->second->Diffuse  = Texture(material.Diffuse );
				if(material.Normal   != "") i->second->Normal   = Texture(material.Normal  );
				if(material.Specular != "") i->second->Specular = Texture(material.Specular);

				i->second->Changed = false;
			}
		}
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto mts = Entity->Get<StorageMaterial>();
			for(auto i = mts.begin(); i != mts.end(); ++i)
			{
				bool Changed = true; // check if the file actually changed
				i->second->Changed = Changed;
			}
		});
	}

	Material Parse(string Path)
	{
		Material material;

		ifstream stream(Name() + "/" + Path);
		if(!stream.is_open())
		{
			Debug::Fail("Material (" + Path + ") cannot be loaded.");
			return material;
		}

		string line;
		while(getline(stream, line))
		{
			istringstream input(line);

			string key;
			input >> key;

			string value;
			vector<string> values;
			while(input >> value) values.push_back(value);
			
			if(key == "") continue;
			else if(key == "newmtl"  ) material.Name     = value;
			else if(key == "Kd"      ) material.Color    = vec3(stof(values[0]), stof(values[1]), stof(values[2]));
			else if(key == "map_Kd"  ) material.Diffuse  = value;
			else if(key == "map_Bump") material.Normal   = value;
			else if(key == "map_Ns"  ) material.Specular = value;
		}

		Debug::Pass("Material load success");		
		return material;
	}

	unsigned int Texture(string Path)
	{
		auto txs = Entity->Get<StorageTexture>();
		for(auto i : txs)
			if(i.second->Path == Path)
				return i.first;

		unsigned int id = Entity->New();
		auto tex = Entity->Add<StorageTexture>(id);
		tex->Path = Path;
		return id;
	}
};
