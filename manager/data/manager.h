#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <stdint.h>

#include <sqlite/sqlite3.h>


class ManagerData
{
public:
	// general
	ManagerData();
	void Name(std::string Path);
	std::string Name();

	// save
	class Serialization;
	template <typename T>
	bool Save(uint64_t Id, T *Instance);
	template <typename T>
	bool Save(std::unordered_map<uint64_t, T*> Instances);

	// load
	class Deserialization;
	template <typename T>
	T *Load(uint64_t Id, T* Instance = new T());
	template <typename T>
	std::unordered_map<uint64_t, T*> Load();
	
private:
	std::string name;

	// type
	template <typename T>
	std::string Type();

	// database
	sqlite3 *Open();
	sqlite3 *Open(std::string Path);
	bool Close(sqlite3 *Database);
	bool Query(sqlite3 *Database, std::string Sql, std::function<void(sqlite3_stmt*)> Callback = [](sqlite3_stmt* statement){});
	bool Table(sqlite3 *Database, std::string Name, std::unordered_map<std::string, std::string> Fields);
};


// include template implementations
#include "save.h"
#include "load.h"
#include "type.h"

// debug
#include "type/person/trait.h"
