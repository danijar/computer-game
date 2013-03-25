#pragma once

#include <functional>
using namespace std;
#include <GLM/glm.hpp>
using namespace glm;


struct StorageText
{
	StorageText() : Text([]{ return ""; }) {}

	function<string(void)> Text;
	uvec2 Position;
};