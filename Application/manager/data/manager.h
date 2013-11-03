#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <stdint.h>

#include "helper/debug.h"

#include <sqlite/sqlite3.h>

#include "manager/data/trait.h"


class ManagerData
{
public:
	bool Test()
	{
		bool result = true;
		sqlite3 *database = Open("save/world.db");
		std::unordered_map<std::string, std::string> fields;
		fields.insert(std::make_pair("id", "INT")); // move this to table function including auto increment and not null
		fields.insert(std::make_pair("value", "FLOAT"));
		result = Table(database, "test", fields) ? true : false;
		result = Close(database) ? result : false;
		return result;
	}
	/*
	template <typename T>
	bool Save(uint64_t Id, T *Instance)
	{
		std::string name = Name<T>();
		sqlite3 *database = Open("save/world.db");

		typedef ManagerDataTrait<T> Trait;
		
		// create vector of column fields
		std::vector<std::string> fields;
		fields.push_back("id");
		for(auto i : Trait::Fields) fields.push_back(i);

		// create table if not exists
		bool table = false;

		ManagerData::Serialization serialization(database, name, fields, table);
		serialization.TEXT("id", std::to_string(Id));
		bool result = Trait::Serialize(Instance, *serialization);
		if(result)
			result = serialization.Execute();

		return result;
	}

	template <typename T>
	T *Load(uint64_t Id)
	{
		std::string name = Name<T>();
		sqlite3 *database = Open("save/world.db");

		typedef ManagerDataTrait<T> Trait;
		
		// create vector of column fields
		std::vector<std::string> fields;
		fields.push_back("id");
		for(auto i : Trait::Fields) fields.push_back(i);

		// create table if not exists
		// ...

		ManagerData::Deserialization deserialization(database, name, fields, Id);
		bool result = deserialization.Execute();
		if(result)
			result = Trait::Deserialize(Instance, *serialization);

		return result;
	}
	*/
	class Serialization
	{
	public:
		Serialization(sqlite3 *Database, std::string Name, std::unordered_map<std::string, std::string> Fields, bool Table = true);
		bool Execute();
		void INTEGER(std::string Field, int Value);
		void FLOAT(std::string Field, float Value);
		void TEXT(std::string Field, std::string Value);
		// add BLOB support
	private:
		std::unordered_map<std::string, std::string> fields;
		sqlite3_stmt *query;
		bool table;
	};

	class Deserialization
	{
	public:
		Deserialization(sqlite3 *Database, std::string Name, std::unordered_map<std::string, std::string> Fields, uint64_t Id = 0);
		bool Execute();
		int INTEGER(std::string Field);
		float FLOAT(std::string Field);
		std::string TEXT(std::string Field);
		// add BLOB support
	private:
		std::unordered_map<std::string, std::string> fields;
		sqlite3_stmt *query;
	};

private:
	/*
	 * Get the lowercase name of the type passed as template argument
	 */
	template <typename T>
	std::string Name()
	{
		// name in the form "struct Name", or "class Name"
		std::string fullname = typeid(T).name();

		// extract last word from name
		std::size_t last = fullname.size();
		while(fullname[last] == ' ' && last > 0) --last;
		if (!last){ HelperDebug::Fail("data", "empty type name"); return ""; }
		std::string name = fullname.substr(fullname.find_last_of(' ', last));

		// transform to lower case
		std::transform(name.begin(), name.end(), name.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

		return name;
	}
	
	// database
	sqlite3 *Open(std::string Path);
	bool Close(sqlite3 *Database);
	bool Query(sqlite3 *Database, std::string Sql, std::function<void(sqlite3_stmt*)> Callback = [](sqlite3_stmt* statement){});
	bool Table(sqlite3 *Database, std::string Name, std::unordered_map<std::string, std::string> Fields);

	// helper
	static std::string Implode(const std::vector<std::string> &Vector, const char* const Separator = ", ");
	static std::string Placeholders(unsigned int Number, const char* const Symbol = "?", const char* const Separator = ", ");
};
