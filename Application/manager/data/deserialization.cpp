#include "manager/data/manager.h"

#include <string>
#include <vector>

using namespace std;


ManagerData::Deserialization::Deserialization(sqlite3 *Database, std::string Name, unordered_map<string, string> Fields, uint64_t Id) : fields(Fields)
{
	/*
	string sql = "SELECT ";
		for(int i = 0; i < fields.size() - 1; ++i) sql += fields[i] + ", ";
		sql += fields.back();
	sql += " FROM " + Name;
	if(Id) sql += " WHERE id = \"" + to_string(Id) + "\"";
	sql += ")";

	sqlite3_prepare(Database, sql.c_str(), -1, &query, 0);
	*/
}

bool ManagerData::Deserialization::Execute()
{
	// run select query
}

int ManagerData::Deserialization::INTEGER(std::string Field)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return 0; }
	int index = i - fields.begin();
	return sqlite3_column_int(query, index);
	*/
}

float ManagerData::Deserialization::FLOAT(std::string Field)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return 0; }
	int index = i - fields.begin();
	return (float)sqlite3_column_double(query, index);
	*/
}

std::string ManagerData::Deserialization::TEXT(std::string Field)
{
	/*
	auto i = find(fields.begin(), fields.end(), Field);
	if(i == fields.end()) { HelperDebug::Fail("data", "field (" + Field + ") does not exist"); return 0; }
	int index = i - fields.begin();
	return std::string(reinterpret_cast<const char*>(sqlite3_column_text(query, index)));
	*/
}
