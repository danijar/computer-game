#include "manager/data/manager.h"

#include <sstream>


std::string ManagerData::Implode(const std::vector<std::string>& Vector, const char* const Separator)
{
	switch (Vector.size())
	{
	case 0:
		return "";
	case 1:
		return Vector[0];
	default:
		std::ostringstream os; 
		std::copy(Vector.begin(), Vector.end() - 1, std::ostream_iterator<std::string>(os, Separator));
		os << *Vector.rbegin();
		return os.str();
	}
}

std::string ManagerData::Placeholders(unsigned int Number, const char* const Symbol, const char* const Separator)
{
	std::string output = "";
	for(unsigned int i = 0; i < Number - 1; ++i)
	{
		output += Symbol;
		output += Separator;
	}
	output += Symbol;
	return output;
}
