#pragma once

#include <string>
#include <unordered_map>

#include <GLEW/glew.h>
#include <SFML/Graphics.hpp>


class ManagerAsset
{
private:
	struct shader { GLuint Program; };
	struct texture { bool Mipmapping, Repeat; GLenum Format; GLuint Id; };
public:
	void Reload()
	{
		// reload assets changed on disk
		// how to access file helper from here?
		// ...
	}
	GLuint Shader(std::string Vertex, std::string Fragment)
	{
		auto i = shaders.find(std::make_pair(Vertex, Fragment));
		if(i != shaders.end())
			return i->second.Program;

		GLuint vertex, fragment, program;
		// create, compile shaders and link program
		// ...

		return program;
	}
	GLuint Texture(std::string Path, GLenum Format = GL_RGB16F, bool Repeat = true, bool Mipmapping = true)
	{
		auto i = textures.find(Path);
		if(i != textures.end())
			if(i->second.Format == Format && i->second.Repeat == Repeat && i->second.Mipmapping == Mipmapping)
				return i->second.Id;
			else
				; // recreate texture object

		GLuint id;
		// create texture and fill it with file
		// ...

		return id;
	}
private:
	std::unordered_map<std::pair<std::string, std::string>, shader> shaders;
	std::unordered_map<std::string, texture> textures;
};
