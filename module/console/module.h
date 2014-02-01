#pragma once

#include "system/system.h"

#include <string>
#include <vector>
#include <v8/v8.h>


class ModuleConsole : public Module
{
	// general
	bool active;
	std::string text;
	void Init();
	void Update();
	void Callbacks();
	void Listeners();

	// history
	std::vector<std::string> history;
	unsigned int history_index;
	void History();

	// callbacks
public:
	static v8::Handle<v8::Value> jsPrint  (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRequire(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsOn     (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsRemove (const v8::Arguments& args);
};
