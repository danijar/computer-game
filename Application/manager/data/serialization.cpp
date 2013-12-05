#include "manager/data/manager.h"

using namespace std;


ManagerData::Serialization::Serialization(sqlite3_stmt *Statement, std::vector<std::string> Fields) : fields(Fields), statement(Statement)
{

}

bool ManagerData::Serialization::INTEGER(string Field, int Value)
{
	int index = Index(Field) + 1;
	if(index < 1) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return false; }

	int result = sqlite3_bind_int(statement, index, Value); // use ?name parameters for simplified binding
	bool success = result == SQLITE_OK;

	if(!success) HelperDebug::Fail("data", "adding integer to (" + Field + ") failed with code (" + to_string(result) + ")");
	return success;
}

bool ManagerData::Serialization::FLOAT(string Field, float Value)
{
	int index = Index(Field) + 1;
	if(index < 1) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return false; }

	int result = sqlite3_bind_double(statement, index, (double)Value);
	bool success = result == SQLITE_OK;

	if(!success) HelperDebug::Fail("data", "adding float to (" + Field + ") failed with code (" + to_string(result) + ")");
	return success;
}
bool ManagerData::Serialization::TEXT(string Field, string Value)
{
	int index = Index(Field) + 1;
	if(index < 1) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return false; }

	int result = sqlite3_bind_text(statement, index, Value.c_str(), -1, SQLITE_TRANSIENT);
	bool success = result == SQLITE_OK;

	if(!success) HelperDebug::Fail("data", "adding string to (" + Field + ") failed with code (" + to_string(result) + ")");
	return success;
}

int ManagerData::Serialization::Index(string Field)
{
	for(unsigned int i = 0; i < fields.size(); ++i)
		if(fields[i] == Field)
			return i;
	return -1;
}
