#pragma once


#define GROUND_TOLERANCE 0.1f

struct Person
{
	Person() : Walking(false), Jumping(false), Changed(true) {}

	void Calculate(float Height)
	{
		this->Height = Height;
		this->Radius = Height / 4;
		this->Step   =   0.250f *  Height;
		this->Eyes   =   0.875f *  Height / 2;
		this->Mass   = 100.000f * (Height - 1);
		this->Step   =   0.250f *  Height;
	}

	float Mass;
	float Height, Radius;
	float Eyes; // distance from center to eyes
	float Step; // maximum height difference for a step

	bool Walking, Jumping;

	bool Changed;
};
