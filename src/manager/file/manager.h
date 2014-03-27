#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include "manager/log/manager.h"


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
		using namespace std;
		using namespace std::tr2::sys;

		// remove trailing slash
		if(Path.back() == '/')
			Path.pop_back();

		// find last path segment
		const size_t position = Path.rfind('/');
		bool slash = (position != string::npos);
		string segment = slash ? Path.substr(position + 1, string::npos) : Path;

		// check wether it is a file
		bool file = (segment.find('.') != string::npos) && (segment.back() != '/');

		// get path without file
		string directory;
		if(!file)
			directory = Path;
		if(file && slash)
			directory = Path.substr(0, position);
		else return true; // no directory in path

		// create directory structure if not exist
		path dir(directory);
		if(!exists(dir)) return create_directories(dir);

		return true;
	}

	/*
	 * Get hash of a file based on file size and modified date.
	 * This is useful for checking if a file has changed.
	 */
	int Hash(std::string Path)
	{
		using namespace std;
		using namespace std::tr2::sys;

		path filepath(Path);
		time_t timestamp = last_write_time(filepath);
		unsigned long long filesize = file_size(filepath);

		return hash<time_t>()(timestamp) + 37 * hash<unsigned long long>()(filesize);
	}

	/*
	 * Read the content of a text file located at the module's directory.
	 */
	std::string Read(std::string Path)
	{
		return Read(name, Path);
	}
	static std::string Read(std::string Name, std::string Path)
	{
		std::ifstream stream(Path);
		if(!stream.is_open())
		{
			ManagerLog::Fail(Name, "cannot read file (" + Path + ")");
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
		Write(name, Path, Text);
	}
	static void Write(std::string Name, std::string Path, std::string Text)
	{
		// ...
	}

private:
	std::string name;
};
