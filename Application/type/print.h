#pragma once

#include <functional>


struct Print
{
	Print() : Text([]{ return ""; }) {}
	std::function<std::string(void)> Text;
};
