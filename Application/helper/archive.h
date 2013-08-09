#pragma once

#include <string>
#include <functional>
#include <future>
#include <filesystem>

#include <ZIP/zipint.h>


class HelperArchive
{
public:
	HelperArchive(std::string Name) : name(Name) { }
	bool Read(std::string Path, std::string File, void *Destination, size_t *Length = nullptr)
	{
		// open archive
		zip *archive = zip_open(Path.c_str(), 0, NULL);
		if(archive == NULL)
			return false;

		// get information about the file
		struct zip_stat stat;
		zip_stat_init(&stat);
		int result = zip_stat(archive, File.c_str(), 0, &stat);
		if(result < 0)
		{
			zip_close(archive);
			return false;
		}

		// open file in archive
		zip_file *file = zip_fopen(archive, File.c_str(), 0);
		if(file == NULL)
		{
			zip_fclose(file);
			zip_close(archive);
			return false;
		}

		// read data into variable
		if(Length) *Length = (size_t)stat.size;
		zip_fread(file, Destination, stat.size);

		// close everything
		zip_fclose(file);
		zip_close(archive);

		return true;
	}
	void ReadAsync(std::string Path, std::string File, void *Destination, size_t *Length, std::function<void(bool)> Callback)
	{
		// later on add to thread queue
		std::async(std::launch::async, [=]{
			bool result = Read(Path, File, Destination, Length);
			// later on add to callback queue to run in main thread
			Callback(result);
		});
	}
	bool Write(std::string Path, std::string File, void *Data, size_t Size)
	{
		// get folder path from file path
		const size_t last_slash = Path.rfind('/');
		if (std::string::npos == last_slash) return false;
		std::string directory = Path.substr(0, last_slash);

		// create folder path if it does not exist
		std::tr2::sys::path dir(directory);
		if(!std::tr2::sys::exists(dir)) std::tr2::sys::create_directories(dir);

		// open archive and create if it does not exist
		zip *archive = zip_open(Path.c_str(), ZIP_CREATE, NULL);
		if(archive == NULL)
			return false;

		// create source buffer from pointer
		struct zip_source *source = zip_source_buffer(archive, Data, Size, 0);
		if(source == NULL)
		{
			zip_source_free(source);
			zip_close(archive);
			return false;
		}

		// write source buffer to archive
		zip_int64_t index = zip_file_add(archive, File.c_str(), source, ZIP_FL_OVERWRITE);
		if(index < 0)
		{
			zip_close(archive);
			return false;
		}

		// write changes to disk
		int result = zip_close(archive);
		if(result < 0)
		{
			HelperDebug::Fail(name, "error closing archive (" + std::string(zip_strerror(archive)) + ")");
			return false;
		}

		return true;
	}
	void WriteAsync(std::string Path, std::string File, void *Data, size_t Size, std::function<void(bool)> Callback = [](bool){})
	{
		std::async(std::launch::async, [=]{
			bool result = Write(Path, File, Data, Size);
			Callback(result);
		});
	}
private:
	std::string name;
};
