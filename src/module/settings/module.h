#pragma once

#include "system/system.h"

#include <sfml/System.hpp>


class ModuleSettings : public Module
{
	// general
	sf::Clock clock;
	int frames;
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// callbacks
public:
	static void jsSettings(const v8::FunctionCallbackInfo<v8::Value> &args);
};
