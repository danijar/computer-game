#include "manager/data/manager.h"

using namespace std;


ManagerData::Serialization::Serialization(sqlite3_stmt *Statement, std::vector<std::string> Fields) : fields(Fields), statement(Statement)
{
	HelperDebug::Print("SERIALIZATION", "set up");
	for(auto i : fields)
		HelperDebug::Inline(i + " (" + to_string(i.size()) + "), ");
	HelperDebug::Print("", "");
}

bool ManagerData::Serialization::INTEGER(string Field, int Value)
{
	int index = Index(Field);
	if(index < 1) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return false; } // just check if(!index)
	int result = sqlite3_bind_int(statement, index, Value); // use ?name parameters for simplified binding
	bool success = result == SQLITE_OK;
	HelperDebug::PassFail("data", "integer added", success, "success", "failed with code (" + to_string(result) + ")");
	return success;
}

bool ManagerData::Serialization::FLOAT(string Field, float Value)
{
	int index = Index(Field);
	if(index < 1) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return false; }
	int result = sqlite3_bind_double(statement, index, (double)Value);
	bool success = result == SQLITE_OK;
	HelperDebug::PassFail("data", "float added", success, "success", "failed with code (" + to_string(result) + ")");
	return success;
}
bool ManagerData::Serialization::TEXT(string Field, string Value)
{
	int index = Index(Field);
	if(index < 1) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return false; }
	int result = sqlite3_bind_text(statement, index, Value.c_str(), Value.size(), SQLITE_TRANSIENT);
	bool success = result == SQLITE_OK;
	HelperDebug::PassFail("data", "text added", success, "success", "failed with code (" + to_string(result) + ")");
	return success;
}

int ManagerData::Serialization::Index(string Field)
{
	HelperDebug::Print("SERIALIZATION", "looking for index of (" + Field + ")");
	for(unsigned int i = 0; i < fields.size(); ++i)
		if(fields[i] == Field)
			return i + 1;
	return -1; // return zero
}
