#pragma once

#include "type.h"
#include "manager/data/manager.h"

using namespace std;


template <>
struct ManagerDataTrait<Person>
{
	static const unordered_map<string, string> Fields;
	/*
	 * When inline member initialization and initializer lists
	 * are available, just use the following line for the above.
	 * std::vector<std::string> Fields = { "height", "mass" };
	 */
	static void Serialize(Person *Instance, ManagerData::Serialization *Data)
	{
		Data->FLOAT("height", Instance->Height);
		Data->FLOAT("mass",   Instance->Mass);
	}
	static void Deserialize(Person *Instance, ManagerData::Deserialization *Data)
	{
		Instance->Height = Data->FLOAT("height");
		Instance->Mass   = Data->FLOAT("mass");
	}
};
