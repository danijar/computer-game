#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "helper/debug.h"


class ManagerData
{
	typedef std::function<std::unordered_map<std::string, std::string>(void*)> Serialization;
	typedef std::function<bool(std::string, void*)> Deserialization;
public:
	template <typename T>
	void Register(std::unordered_map<std::string, std::string> Structure, Serialization Serialize, Deserialization Deserialize)
	{
		std::string table = typeid(T).name();
		auto key = std::type_index(typeid(T));

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

		registered.insert(std::make_pair(key, std::make_pair(Serialize, Deserialize)));
	}
	template <typename T>
	bool Save(unsigned int Id, T *Instance)
	{
		std::string table = typeid(T).name();
		auto key = std::type_index(typeid(T));

		auto i = registered.find(key);
		if(i == registered.end())
		{
			HelperDebug::Fail("data", "no serialization function for " + table);
			return false;
		}
		auto serialized = i->first(Instance);

		std::string keys = "id", values = to_string(Id);
		for(auto i : serialized)
			keys += ", " + i.first, values += ", " + i.second;

		std::string query = "INSERT OR REPLACE INTO " + table + " (" + keys + ") VALUES (" + values + ");"
		// execute query
		// return false on fail

		return true;
	}
	template <typename T>
	bool Load(unsigned int id, T *Instance)
	{
		std::string table = typeid(T).name();
		auto key = std::type_index(typeid(T));

		auto i = registered.find(key);
		if(i == registered.end())
		{
			HelperDebug::Fail("data", "no deserialization function for " + table);
			return false;
		}

		std::string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + ";";
		// execute query;
		std::string row;

		bool result = i->second(row, Instance);
		return result;
	}
private:
	std::unordered_map<std::type_index, std::pair<Serialization, Deserialization>> registered;
};
