#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <stdint.h>

#include <sqlite/sqlite3.h>

#include "manager/data/trait.h"
#include "helper/debug.h"


class ManagerData
{
public:
	ManagerData();

	template <typename T>
	bool Save(uint64_t Id, T *Instance)
	{
		std::string name = Name<T>();
		sqlite3 *database = Open(path);

		typedef ManagerDataTrait<T> Trait;

		Table(database, name, Trait::Fields);

		std::vector<std::string> fields;
		fields.reserve(Trait::Fields.size() + 1);
		fields.push_back("id");
		
		std::string sql = "INSERT OR REPLACE";
		sql += " INTO " + name + " (id";
		for(auto i : Trait::Fields)
		{
			sql += ", " + i.first;
			fields.push_back(i.first);
		}
		sql += ") VALUES (?";
		for(unsigned int i = 0; i < Trait::Fields.size(); ++i)
			sql += ", ?";
		sql += ")";

		sqlite3_stmt *statement;
		sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
		Serialization serialization(statement, fields);
		serialization.TEXT("id", std::to_string(Id));

		Trait::Serialize(Instance, &serialization);

		bool fine;
		int result = sqlite3_step(statement);
		if(result == SQLITE_DONE) fine = true;
		else { HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")"); fine = false; }

		Close(database); // consider keeping up the connection for performance
		return fine;
	}

	template <typename T>
	bool Save(std::unordered_map<uint64_t, T*> Instances)
	{
		std::string name = Name<T>();
		sqlite3 *database = Open(path);

		typedef ManagerDataTrait<T> Trait;

		Table(database, name, Trait::Fields);

		std::vector<std::string> fields;
		fields.reserve(Trait::Fields.size() + 1);
		fields.push_back("id");
		
		std::string sql = "INSERT OR REPLACE";
		sql += " INTO " + name + " (id";
		for(auto i : Trait::Fields)
		{
			sql += ", " + i.first;
			fields.push_back(i.first);
		}
		sql += ") VALUES (?";
		for(unsigned int i = 0; i < Trait::Fields.size(); ++i)
			sql += ", ?";
		sql += ")";

		sqlite3_stmt *statement;
		sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
		Serialization serialization(statement, fields);

		for(auto i : Instances)
		{
			serialization.TEXT("id", std::to_string(Id));
			Trait::Serialize(Instance, &serialization);

			while((result = sqlite3_step(statement)) == SQLITE_BUSY);
			if(result == SQLITE_DONE)
			{
				HelperDebug::Pass("data", "stored row in (" + name + ")"); // debug
			}
			else
			{
				HelperDebug::Fail("data", "query in (" + name + ") failed with code (" + to_string(result) + ")");
				Close(database);
				return false;
			}

			sqlite3_reset(statement);
		}

		Close(database); // consider keeping up the connection for performance
		return true;
	}


	template <typename T>
	T *Load(uint64_t Id, T* Instance = new T())
	{
		std::string name = Name<T>();
		sqlite3 *database = Open(path);

		typedef ManagerDataTrait<T> Trait;
		
		std::vector<std::string> fields;
		fields.reserve(Trait::Fields.size() + 1);
		fields.push_back("id");

		std::string sql = "SELECT id";
		for(auto i : Trait::Fields)
		{
			sql += ", " + i.first;
			fields.push_back(i.first);
		}
		sql += " FROM " + name;
		sql += " WHERE id = '" + std::to_string(Id) + "'";

		sqlite3_stmt *statement;
		sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
		Deserialization deserialization(statement, fields);

		int result;
		while((result = sqlite3_step(statement)) == SQLITE_BUSY);
		if(result == SQLITE_ROW)
		{
			HelperDebug::Pass("data", "found row from (" + name + ")"); // debug
			Trait::Deserialize(Instance, &deserialization);
		}
		else if(result == SQLITE_DONE) HelperDebug::Pass("data", "no row found");
		else HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")");

		Close(database);
		return Instance;
	}

	template <typename T>
	std::unordered_map<uint64_t, T*> Load()
	{
		std::string name = Name<T>();
		sqlite3 *database = Open(path);

		typedef ManagerDataTrait<T> Trait;
		
		std::vector<std::string> fields;
		fields.reserve(Trait::Fields.size() + 1);
		fields.push_back("id");

		std::string sql = "SELECT id";
		for(auto i : Trait::Fields)
		{
			sql += ", " + i.first;
			fields.push_back(i.first);
		}
		sql += " FROM " + name;

		HelperDebug::Pass("data", "sql (" + sql + ")");

		sqlite3_stmt *statement;
		sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
		Deserialization deserialization(statement, fields);

		std::unordered_map<uint64_t, T*> instances;
		while(int result = sqlite3_step(statement))
		{
			if(result == SQLITE_BUSY) continue;
			else if(result == SQLITE_ROW)
			{
				HelperDebug::Pass("data", "found a row from (" + name + ")"); // debug
				T* instance = new T();
				uint64_t id = deserialization.TEXT("id");
				Trait::Deserialize(instance, &deserialization);
				instances.insert(std::make_pair(id, instance));
			}
			else if(result == SQLITE_DONE) break;
			else { HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")"); break; }
		};

		Close(database);
		return instances;
	}

	class Serialization
	{
	public:
		Serialization(sqlite3_stmt *Statement, std::vector<std::string> Fields);
		bool INTEGER(std::string Field, int Value);
		bool FLOAT(std::string Field, float Value);
		bool TEXT(std::string Field, std::string Value);
		// add BLOB support
	private:
		int Index(std::string Field);
		std::vector<std::string> fields;
		sqlite3_stmt *statement;
	};

	class Deserialization
	{
	public:
		Deserialization(sqlite3_stmt *Statement, std::vector<std::string> Fields);
		int INTEGER(std::string Field);
		float FLOAT(std::string Field);
		std::string TEXT(std::string Field);
		// add BLOB support
	private:
		int Index(std::string Field);
		std::vector<std::string> fields;
		sqlite3_stmt *statement;
	};

	// name
	void Name(std::string Path);
private:
	std::string path;

	/*
	 * Get the lowercase name of the type passed as template argument
	 */
	template <typename T>
	std::string Name()
	{
		// name in the form "struct Name", or "class Name"
		std::string type = typeid(T).name();

		// extract last word from name
		while(type.back() == ' ') type.pop_back();
		unsigned int i = type.size() - 1;
		while(type.at(i) != ' ') i--;
		string name = type.substr(i + 1);

		// transform to lower case
		std::transform(name.begin(), name.end(), name.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

		return name;
	}

	// database
	sqlite3 *Open(std::string Path);
	bool Close(sqlite3 *Database);
	bool Query(sqlite3 *Database, std::string Sql, std::function<void(sqlite3_stmt*)> Callback = [](sqlite3_stmt* statement){});
	bool Table(sqlite3 *Database, std::string Name, std::unordered_map<std::string, std::string> Fields);
};
