#pragma once

#include "system/system.h"

#include "dependency/sfml/System.hpp"


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
	static v8::Handle<v8::Value> jsSettings(const v8::Arguments& args);
};
