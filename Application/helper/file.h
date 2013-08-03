#pragma once

#include <string>
#include <fstream>

#include "helper/debug.h"


class HelperFile
{
public:
	HelperFile(std::string Name) : name(Name) {}

	std::string Read(std::string Path)
	{
		return Read("module/" + name, Path);
	}
	void Write(std::string Path, std::string Text)
	{
		Write("module/" + name, Path, Text);
	}

	static void Write(std::string Name, std::string Path, std::string Text)
	{
		std::string path = Name + "/" + Path;
		// write here
	}
	static std::string Read(std::string Name, std::string Path)
	{
		std::string path = Name + "/" + Path;
		std::ifstream stream(path);
		if(!stream.is_open())
		{
			HelperDebug::Fail(Name, "cannot read file (" + Path + ")");
			return "";
		}
		return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	}
private:
	std::string name;
};
