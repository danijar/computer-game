#include "type.h"
#include "trait.h"


const std::unordered_map<std::string, std::string> ManagerDataTrait<Person>::Fields = []{
	std::unordered_map<std::string, std::string> fields;
	fields.insert(std::make_pair("height", "FLOAT"));
	fields.insert(std::make_pair("mass", "FLOAT"));
	return fields;
}();
