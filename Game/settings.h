#pragma once

#include <string>
using namespace std;

#include "system.h"

struct StorageSettings : public Storage
{
	bool Fullscreen;
	int Width, Height;
	string Title;
};