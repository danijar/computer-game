#pragma once

#include "manager/data/trait.h"


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

/*
template <>
struct ManagerDataTrait<Person>
{
	ManagerDataTrait()
	{
		Fields.insert(std::make_pair("height", "FLOAT"));
		Fields.insert(std::make_pair("mass", "FLOAT"));
	}
	std::unordered_map<std::string, std::string> Fields;
	/*
	 * When inline member initialization and initializer lists
	 * are available, just use the following line for the above.
	 * std::vector<std::string> Fields = { "height", "mass" };
	 */
	/*
	bool Serialize(Person *Instance, ManagerData::Serialization *Data)
	{
		Data->FLOAT("height", Instance->Height);
		Data->FLOAT("mass",   Instance->Mass);
	}
	bool Deserialize(Person *Instance, ManagerData::Deserialization *Data)
	{
		Instance->Height = Data->FLOAT("height");
		Instance->Mass   = Data->FLOAT("mass");
	}
};
*/
