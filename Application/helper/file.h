#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include "helper/debug.h"


class HelperFile
{
public:
	HelperFile(std::string Name) : name(Name) {}

	/*
	 * Creates a directory structure if not exists.
	 * Return false on failure.
	 */
	static bool Create(std::string Path) // detect files by extension dot and ignore last segment
	{
		// remove trailing slash
		if(Path.back() == '/')
			Path.pop_back();

		// find last path segment
		const size_t position = Path.rfind('/');
		bool slash = (position != std::string::npos);
		std::string segment = slash ? Path.substr(position + 1, std::string::npos) : Path;

		// check wether it is a file
		bool file = (segment.find('.') != std::string::npos) && (segment.back() != '/');

		// get path without file
		std::string directory;
		if(!file)
			directory = Path;
		if(file && slash)
			directory = Path.substr(0, position);
		else return true; // no directory in path

		// create directory structure if not exist
		std::tr2::sys::path dir(directory);
		if(!std::tr2::sys::exists(dir)) return std::tr2::sys::create_directories(dir);

		return true;
	}

	/*
	 * Get hash of a file based on file size and modified date.
	 * This is useful for checking if a file has changed.
	 */
	int Hash(std::string Path)
	{
		std::tr2::sys::path filepath(Path);
		std::time_t timestamp = std::tr2::sys::last_write_time(filepath);
		unsigned long long filesize = std::tr2::sys::file_size(filepath);

		return std::hash<time_t>()(timestamp) + 37 * std::hash<unsigned long long>()(filesize);
	}

	/*
	 * Read the content of a text file located at the module's directory.
	 */
	std::string Read(std::string Path)
	{
		return Read("module/" + name, Path);
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

	/*
	 * Write a text file located at the module's directory.  If the file doesn't
	 * exist, it will be created wirst. Otherwise the content will be overridden.
	 */
	void Write(std::string Path, std::string Text)
	{
		Write("module/" + name, Path, Text);
	}
	static void Write(std::string Name, std::string Path, std::string Text)
	{
		std::string path = Name + "/" + Path;
		// write here
	}

private:
	std::string name;
};
