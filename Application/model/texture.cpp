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
		int Count = 0;
		for(auto i = txs.begin(); i != txs.end(); ++i)
		{
			if(i->second->Changed)
			{
				Load(i->first);
				i->second->Changed = false;
				Count++;
			}
		}
		if(Count > 0)
		{
			Debug->Print("reloaded " + to_string(Count));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Listeners()
	{
		Event->Listen("WindowFocusGained", [=]{
			auto txs = Entity->Get<StorageTexture>();
			int Count = 0;
			for(auto i = txs.begin(); i != txs.end(); ++i)
			{
				i->second->Changed = true; // check if the file actually changed
			}
		});
	}

	void Load(unsigned int Id)
	{
		auto tex = Entity->Get<StorageTexture>(Id);

		Image image;
		bool result = image.loadFromFile("model/" + Name() + "/" + tex->Path);
		if(!result)
		{
			Debug->Fail("(" + tex->Path + ") cannot be loaded.");
			return;
		}
		image.flipVertically();

		glBindTexture(GL_TEXTURE_2D, tex->Id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if(tex->Mipmapping)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
	}
};
