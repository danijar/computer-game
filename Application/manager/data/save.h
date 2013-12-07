#pragma once

#include "manager.h"
#include "trait.h"

#include "helper/debug.h"


class ManagerData::Serialization
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

template <typename T>
bool ManagerData::Save(uint64_t Id, T *Instance)
{
	std::string name = Type<T>();
	sqlite3 *database = Open();
	if(!database) return false;

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

	int result = sqlite3_step(statement);
	bool success = (result == SQLITE_DONE);
	if(!success)
		HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")");

	Close(database); // consider keeping up the connection for performance
	return success;
}

template <typename T>
bool ManagerData::Save(std::unordered_map<uint64_t, T*> Instances)
{
	std::string name = Type<T>();
	sqlite3 *database = Open();
	if(!database) return false;

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

	int result;
	for(auto i : Instances)
	{
		serialization.TEXT("id", std::to_string(i.first));
		Trait::Serialize(i.second, &serialization);

		while((result = sqlite3_step(statement)) == SQLITE_BUSY);
		if(result != SQLITE_DONE)
		{
			HelperDebug::Fail("data", "query in (" + name + ") failed with code (" + to_string(result) + ")");
			Close(database);
			return false;
		}

		sqlite3_reset(statement);
	}

	Close(database);
	return true;
}
