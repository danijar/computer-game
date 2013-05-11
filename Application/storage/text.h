#pragma once

#include <functional>


struct StorageText
{
	StorageText() : Text([]{ return ""; }) {}
	std::function<std::string(void)> Text;
};
