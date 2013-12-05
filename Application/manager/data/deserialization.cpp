#include "manager/data/manager.h"

using namespace std;


ManagerData::Deserialization::Deserialization(sqlite3_stmt *Statement, vector<string> Fields) : fields(Fields), statement(Statement)
{
	HelperDebug::Pass("data", "deserializataion initialization");
	for(auto i : fields)
		HelperDebug::Inline(i + ", ");
	HelperDebug::Pass("", "");
}

int ManagerData::Deserialization::INTEGER(string Field)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return 0; }
	HelperDebug::Print("data", "deserialize integer (" + Field + ") at index (" + to_string(index) + ")");
	return sqlite3_column_int(statement, index);
}

float ManagerData::Deserialization::FLOAT(string Field)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return 0.f; }
	HelperDebug::Print("data", "deserialize float (" + Field + ") at index (" + to_string(index) + ")");
	return (float)sqlite3_column_double(statement, index);
}

string ManagerData::Deserialization::TEXT(string Field)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field (" + Field + ") does not exist"); return ""; }
	HelperDebug::Print("data", "deserialize text (" + Field + ") at index (" + to_string(index) + ")");
	return std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, index)));
}

int ManagerData::Deserialization::Index(string Field)
{
	for(unsigned int i = 0; i < fields.size(); ++i)
		if(fields[i] == Field)
			return i;
	return -1;
}
