#include "manager.h"

#include "manager/log/manager.h"

using namespace std;


ManagerData::Deserialization::Deserialization(sqlite3_stmt *Statement, vector<string> Fields) : fields(Fields), statement(Statement)
{

}

int ManagerData::Deserialization::INTEGER(string Field)
{
	int index = Index(Field);
	if(index < 0) { ManagerLog::Warning("data", "field (" + Field + ") does not exist"); return 0; }
	return sqlite3_column_int(statement, index);
}

float ManagerData::Deserialization::FLOAT(string Field)
{
	int index = Index(Field);
	if(index < 0) { ManagerLog::Warning("data", "field (" + Field + ") does not exist"); return 0.f; }
	return (float)sqlite3_column_double(statement, index);
}

string ManagerData::Deserialization::TEXT(string Field)
{
	int index = Index(Field);
	if(index < 0) { ManagerLog::Warning("data", "field (" + Field + ") does not exist"); return ""; }
	return std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, index)));
}

int ManagerData::Deserialization::Index(string Field)
{
	for(unsigned int i = 0; i < fields.size(); ++i)
		if(fields[i] == Field)
			return i;
	return -1;
}
