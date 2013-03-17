#pragma once

#include "system.h"

#include <string>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>
using namespace sf;

#include "texture.h"


class ModuleTexture : public Module
{
	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto txs = Entity->Get<StorageTexture>();
		for(auto i = txs.begin(); i != txs.end(); ++i)
		{
			if(i->second->Changed)
			{
				glBindTexture(GL_TEXTURE_2D, i->second->Id);
				Load(Name() + "/" + i->second->Path);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				if(i->second->Mipmapping)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				i->second->Changed = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto txs = Entity->Get<StorageTexture>();
			for(auto i = txs.begin(); i != txs.end(); ++i)
			{
				bool Changed = true; // check if the file actually changed
				i->second->Changed = Changed;
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		});
	}

	void Load(string Path)
	{
		Image image;
		bool result = image.loadFromFile(Path);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	}
};
