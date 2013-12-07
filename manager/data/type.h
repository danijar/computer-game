#pragma once

#include "manager.h"


/*
 * Get the lowercase name of the type passed as template argument
 */
template <typename T>
std::string ManagerData::Type()
{
	// name in the form "struct Name", or "class Name"
	std::string type = typeid(T).name();

	// extract last word from name
	while(type.back() == ' ') type.pop_back();
	unsigned int i = type.size() - 1;
	while(type.at(i) != ' ') i--;
	string name = type.substr(i + 1);

	// transform to lower case
	std::transform(name.begin(), name.end(), name.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

	return name;
}
