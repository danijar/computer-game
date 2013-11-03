#pragma once

//#include "manager/data/trait.h"
#include "manager/data/manager.h"

#define GROUND_TOLERANCE 0.1f

struct Person
{
	Person() : Walking(false), Jumping(false), Changed(true) {}

	void Calculate(float Height)
	{
		this->Height =            Height;
		this->Step   =   0.25f *  Height;
		this->Eyes   =   1.10f *  Height / 2;
		this->Mass   = 100.00f * (Height - 1);
		this->Radius =            Height / 4;
		this->Step   =   0.25f *  Height;
	}

	float Mass;
	float Radius, Height;
	float Eyes; // distance from center to eyes
	float Step; // maximum height difference for a step

	bool Walking, Jumping;

	bool Changed;
};


// move this in separate file later
template <>
struct ManagerDataTrait<Person>
{
	static const std::unordered_map<std::string, std::string> Fields;
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

const std::unordered_map<std::string, std::string> ManagerDataTrait<Person>::Fields = []{
	std::unordered_map<std::string, std::string> fields;
	fields.insert(std::make_pair("height", "FLOAT"));
	fields.insert(std::make_pair("mass", "FLOAT"));
	return fields;
}();
