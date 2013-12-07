#pragma once

#include "manager.h"
#include "trait.h"

#include "helper/debug.h"


class ManagerData::Deserialization
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

template <typename T>
T *ManagerData::Load(uint64_t Id, T* Instance)
{
	using namespace std;

	string name = Type<T>();
	sqlite3 *database = Open();
	if(!database) return Instance;

	typedef ManagerDataTrait<T> Trait;
		
	vector<string> fields;
	fields.reserve(Trait::Fields.size() + 1);
	fields.push_back("id");

	string sql = "SELECT id";
	for(auto i : Trait::Fields)
	{
		sql += ", " + i.first;
		fields.push_back(i.first);
	}
	sql += " FROM " + name;
	sql += " WHERE id = '" + to_string(Id) + "'";

	sqlite3_stmt *statement;
	sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
	Deserialization deserialization(statement, fields);

	int result;
	while((result = sqlite3_step(statement)) == SQLITE_BUSY);
	if(result == SQLITE_ROW)
		Trait::Deserialize(Instance, &deserialization);
	else if(result == SQLITE_DONE)
		HelperDebug::Warning("data", "no row found");
	else
		HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")");

	Close(database);
	return Instance;
}

template <typename T>
std::unordered_map<uint64_t, T*> ManagerData::Load()
{
	using namespace std;

	string name = Type<T>();
	sqlite3 *database = Open();
	if(!database) return unordered_map<uint64_t, T*>();

	typedef ManagerDataTrait<T> Trait;
		
	vector<string> fields;
	fields.reserve(Trait::Fields.size() + 1);
	fields.push_back("id");

	string sql = "SELECT id";
	for(auto i : Trait::Fields)
	{
		sql += ", " + i.first;
		fields.push_back(i.first);
	}
	sql += " FROM " + name;

	sqlite3_stmt *statement;
	sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0);
	Deserialization deserialization(statement, fields);

	unordered_map<uint64_t, T*> instances;
	while(int result = sqlite3_step(statement))
	{
		if(result == SQLITE_BUSY) continue;
		else if(result == SQLITE_ROW)
		{
			T* instance = new T();
			uint64_t id = stoull(deserialization.TEXT("id"));
			Trait::Deserialize(instance, &deserialization);
			instances.insert(make_pair(id, instance));
		}
		else if(result == SQLITE_DONE) break;
		else { HelperDebug::Fail("data", "query from (" + name + ") failed with code (" + to_string(result) + ")"); break; }
	};

	Close(database);
	return instances;
}
