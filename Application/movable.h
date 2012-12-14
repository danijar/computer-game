#pragma once

#include "system.h"

#include <vector>
using namespace std;
#include <GLM/glm.hpp>
using namespace glm;


struct StorageMovables : public Storage
{
	struct Movable 
	{
		// vec3 Momentum;
	};

	vector<Movable> List;
};