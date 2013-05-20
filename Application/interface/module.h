#pragma once

#include "system.h"

#include <SFML/Graphics.hpp>


class ModuleInterface : public Module
{
	// general
	bool show;
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// print
	sf::Font font;
	void DrawPrint();

public:
	// callbacks
	static v8::Handle<v8::Value> jsInterface(const v8::Arguments& args);
};
