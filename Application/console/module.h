#pragma once

#include "system.h"

#include <string>
#include <vector>
#include <V8/v8.h>


class ModuleConsole : public Module
{
	void Init();
	void Update();
	void Listeners();

	bool active;
	std::string text;

	std::vector<std::string> history;
	unsigned int history_index;
	void History();
public:
	static v8::Handle<v8::Value> jsPrint(const v8::Arguments& args);
};
