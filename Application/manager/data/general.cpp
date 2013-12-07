#include "manager.h"

using namespace std;


ManagerData::ManagerData()
{
	Name("world");
}

void ManagerData::Name(string Path)
{
	name = Path;
}

string ManagerData::Name()
{
	return name;
}
