#pragma once

#include "system/system.h"

#include <string>
#include <sfml/Window.hpp>

class ModuleWindow : public Module
{
	// general
	void Init();
	void Update();
	void Listeners();
	void Callbacks();

	// create
	void Create();
	void Create(bool Fullscreen);
	void Close();

	// keyboard
	sf::Keyboard::Key Key(std::string Name);
	bool Pressed(std::vector<sf::Keyboard::Key> Keys);

public:
	// callbacks
	static void jsTitle     (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsVsync     (const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsWindowsize(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsKey       (const v8::FunctionCallbackInfo<v8::Value> &args);
};
