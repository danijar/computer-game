#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <locale>
#include <typeindex>

#include <sqlite/sqlite3.h>

#include "helper/debug.h"


class HelperData
{
	/*
	 * Shortcut for a map with database field names as keys and their values.
	 */
	typedef std::unordered_map<std::string, std::string> string_map;

	/*
	 * Function to extract data from property struct to a map for save in database.
	 * Parameters is void pointer to property instance, which must be casted back.
	 * Return type type is a map with database field names as keys and their values.
	 */
	typedef std::function<string_map(void*)> Serialization;

	/*
	 * Function to fill property struct with data from database.
	 * Parameters are void pointer to property instance, which must be casted back,
	 * and a map with database field names as keys and their values.
	 * Return type indicates success.
	 */
	typedef std::function<bool(void*, string_map)> Deserialization;

	/*
	 * Value type for the list of registered serialization and deserialization functions.
	 */
	struct Functions
	{
		Functions(Serialization Serialize, Deserialization Deserialize) : Serialize(Serialize), Deserialize(Deserialize) { }
		Serialization Serialize;
		Deserialization Deserialize;
	};

public:
	/* 
	 * Just for development purpose.
	 * Later on remove this function.
	 */
	bool Test()
	{
		sqlite3 *connection = Open("save/database.db");
		if(!connection) return false;
		bool result = Close(connection);
		HelperDebug::Pass("data", "test successful");
		return result;
	}
	/*
	 * Register a property type to the manager, providing its structure in database and converter functions.
	 * Parameters are a map with future database field names as keys and their type names as value,
	 * and functions to read important data from struct to a map and from a map into the struct again.
	 * Return type indicates success.
	 */
	template <typename T>
	bool Register(std::unordered_map<std::string, std::string> Structure, Serialization Serialize, Deserialization Deserialize)
	{
		std::string table = Name<T>();
		auto key = std::type_index(typeid(T));

		if(registered.find(key) != registered.end())
		{
			HelperDebug::Fail("data", "property (" + table + ") is already registered");
			return false;
		}

		std::string query = "CREATE TABLE IF NOT EXISTS " + table + " (";
		bool first = true;
		for(auto i : Structure)
		{
			if(first) first = false;
			else query += ", ";
			query += i.first + " " + i.second;
		}
		query += ");";

		// execute query...

		registered.insert(std::make_pair(key, Functions(Serialize, Deserialize)));
		return true;
	}

	/*
	 *
	 */
	template <typename T>
	bool Save(uint64_t Id, T *Instance)
	{
		std::string table = Name<T>();
		auto key = std::type_index(typeid(T));

		auto i = registered.find(key);
		if(i == registered.end())
		{
			HelperDebug::Fail("data", "no serialization function for (" + table + ")");
			return false;
		}
		string_map serialized = i->second.Serialize(Instance);

		std::string keys = "id", values = to_string(Id);
		for(auto i : serialized)
			keys += ", " + i.first, values += ", " + i.second;

		std::string query = "INSERT OR REPLACE INTO " + table + " (" + keys + ") VALUES (" + values + ");";
		// execute query,
		// and return false on fail

		// provide example output
		HelperDebug::Pass("data", query);

		return true;
	}

	/*
	 *
	 */
	template <typename T>
	bool Load(uint64_t id, T *Instance)
	{
		std::string table = Name<T>();
		auto key = std::type_index(typeid(T));

		auto i = registered.find(key);
		if(i == registered.end())
		{
			HelperDebug::Fail("data", "no deserialization function for (" + table + ")");
			return false;
		}

		std::string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + ";";
		// execute query,
		// and store result in this variable
		string_map serialized;

		// provide example output and data for testing
		HelperDebug::Pass("data", query);
		for(int i = 1; i <= 4;++i)
			serialized.insert(std::make_pair("key" + std::to_string(i), "value" + std::to_string(i)));

		bool result = i->second.Deserialize(Instance, serialized);
		return result;
	}

	/*
	 * Load all properties of a given type from database and fill their values in newly allocated structs.
	 * Parameter is a function that gets executed for each property in database to fetch or create the related property struct.
	 * Return type is a map with entity ids as key and newly created instances as value, like what the analogous entity manager function returns.
	 */
	template <typename T>
	std::unordered_map<uint64_t, T*> Load(std::function<T*(uint64_t)> Walker)
	{

	}
	/*
	 * Shortcut for default walker function that always creates a new instance.
	 * Useful for loading a dataset for the first time.
	 */
	template <typename T>
	std::unordered_map<uint64_t, T*> Load()
	{
		Load([]{ return new T(); });
	}

private:
	/* static */ std::unordered_map<std::type_index, Functions> registered;

	/*
	 * Get the lowercase name of the type passed as template argument
	 */
	template <typename T>
	std::string Name()
	{
		// name in form "struct Name", or "class Name"
		std::string fullname = typeid(T).name();

		// extract last word from name
		std::size_t last = fullname.size();
		while(fullname[last] == ' ' && last > 0) --last;
		if (last == 0)
		{
			HelperDebug::Fail("data", "empty type name for some reason");
			return "";
		}
		std::string name = fullname.substr(fullname.find_last_of(' ', last));

		// transform to lower case
		std::transform(name.begin(), name.end(), name.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

		return name;
	}
	/*
	 * Connects to a database.
	 * If the database specified by the path does not exist, a new one is created.
	 */
	sqlite3 *Open(std::string Path)
	{
		// create directory structure if not exist
		// ...

		sqlite3 *database = nullptr; // need to use "new"?
		int result = sqlite3_open_v2(Path.c_str(), &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		if(result != SQLITE_OK)
			HelperDebug::Fail("data", "connect to database fail");
		return database;
	}
	/*
	 * Closes the connection to a datavase.
	 * The connection handle gets deleted.
	 */
	bool Close(sqlite3 *Connection)
	{
		int result = sqlite3_close_v2(Connection);
		if(result != SQLITE_OK)
		{
			HelperDebug::Fail("data", "disconnect from database failed");
			return false;
		}
		// delete Connection;
		return true;
	}
};
