#include "module.h"

#include <string>
#include <unordered_map>
#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


GLuint ModuleModel::GetTexture(string Path)
{
	auto i = textures.find(Path);
	if(i != textures.end()) return i->second.first;

	GLuint texture;
	glGenTextures(1, &texture);
	LoadTexture(texture, Path);

	textures.insert(make_pair(Path, make_pair(texture, Hash("module/" + Name() + "/texture/" + Path))));
	return texture;
}

void ModuleModel::ReloadTextures()
{
	for(auto i = textures.begin(); i != textures.end(); ++i)
	{
		int hash = Hash("module/" + Name() + "/texture/" + i->first);
		if(i->second.second != hash)
		{
			i->second.second = hash;
			LoadTexture(i->second.first, i->first);
			Debug->Pass("texture (" + i->first + ") reloaded");
		}
	}
}

void ModuleModel::LoadTexture(GLuint &Texture, string Path)
{
	Image image;
	bool result = image.loadFromFile("module/" + Name() + "/texture/" + Path);
	if(!result)
	{
		Debug->Fail("texture (" + Path + ") cannot be loaded");
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
