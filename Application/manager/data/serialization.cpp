#include "manager/data/manager.h"

#include <string>
#include <vector>

using namespace std;


ManagerData::Serialization::Serialization(sqlite3 *Database, string Name, unordered_map<string, string> Fields, bool Table) : fields(Fields), table(Table)
{
	/*
	// move this to execute(),
	// for know just collect information provided by the serialization trait
	string sql = "INSERT OR REPLACE INTO " + Name + "(";
		for(int i = 0; i < fields.size() - 1; ++i) sql += fields[i] + ", ";
		sql += fields.back();
	sql += ") VALUES (";
		for(int i = 0; i < fields.size() - 1; ++i) sql += "?, ";
		sql += "?";
	sql += ");";

	sqlite3_prepare(Database, sql.c_str(), -1, &query, 0);
	*/
}

bool ManagerData::Serialization::Execute()
{
	if(!table)
	{
		// create table
	}

	// run insert query
}

void ManagerData::Serialization::INTEGER(string Field, int Value)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return; }
	int index = i - fields.begin();

	if(!table)
	{
		// add "INT Field" to table query
	}

	sqlite3_bind_int(query, index, Value);
	*/
}

void ManagerData::Serialization::FLOAT(string Field, float Value)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return; }
	int index = i - fields.begin();

	if(!table)
	{
		// add "FLOAT Field" to table query
	}

	sqlite3_bind_double(query, index, (double)Value);
	*/
}
void ManagerData::Serialization::TEXT(string Field, string Value)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return; }
	int index = i - fields.begin();

	if(!table)
	{
		// add "TEXT Field" to table query
	}

	sqlite3_bind_text(query, index, Value.c_str(), -1, SQLITE_STATIC);
	*/
}
