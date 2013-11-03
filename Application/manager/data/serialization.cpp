#include "manager/data/manager.h"

using namespace std;


ManagerData::Serialization::Serialization(sqlite3_stmt *Statement, std::vector<std::string> Fields) : fields(Fields)
{

}

void ManagerData::Serialization::INTEGER(string Field, int Value)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return; }
	sqlite3_bind_int(statement, index, Value);
}

void ManagerData::Serialization::FLOAT(string Field, float Value)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return; }
	sqlite3_bind_double(statement, index, (double)Value);
}
void ManagerData::Serialization::TEXT(string Field, string Value)
{
	int index = Index(Field);
	if(index < 0) { HelperDebug::Warning("data", "field name (" + Field + ") does not exist"); return; }
	sqlite3_bind_text(statement, index, Value.c_str(), -1, SQLITE_STATIC);
}

int ManagerData::Serialization::Index(string Field)
{
	for(unsigned int i = 0; i < fields.size(); ++i)
		if(fields[i] == Field)
			return i;
	return -1;
}
