#pragma once

#include "system/system.h"


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

public:
	// callbacks
	static v8::Handle<v8::Value> jsTitle     (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsVsync     (const v8::Arguments& args);
	static v8::Handle<v8::Value> jsWindowsize(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsKey       (const v8::Arguments& args);
};
