#pragma once

#include <string>
#include <unordered_map>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>
using namespace sf;


class Textures
{
public:
	GLuint Get(string Path)
	{
		auto i = list.find(Path);
		if(i != list.end()) return i->second;

		GLuint texture;
		glGenTextures(1, &texture);
		Load(texture, Path);

		list.insert(make_pair(Path, texture));
		return texture;
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
	void Load(GLuint &Texture, string Path)
	{
		Image image;
		bool result = image.loadFromFile("model/texture/" + Path);
		if(!result)
		{
			// Debug->Fail("(" + Path + ") cannot be loaded.");
			return;
		}
		image.flipVertically();

		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unordered_map<string, GLuint> list;
};
