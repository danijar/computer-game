#pragma once

#include "system.h"

#include <iostream>
using namespace std;

struct Integer : public Storage
{
	int Value;
};

struct Position : public Storage
{
	int X, Y, Z;
};

class Physics : public Component
{
public:
	void Update();
	void Init();
};