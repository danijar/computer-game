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
	static void jsPrint  (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsRequire(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsOn     (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsRemove (const v8::FunctionCallbackInfo<v8::Value> &args);
};
