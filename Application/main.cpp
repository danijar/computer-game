#pragma once

#include "system.h"
#include "debug.h"

#include "settings.cpp"
#include "window.cpp"
#include "input.cpp"
#include "renderer.cpp"
#include "form.cpp"


int main()
{
	System World;

	World.Add("settings",	new ComponentSettings(),	Input);
	World.Add("window",		new ComponentWindow(),		Input);
	World.Add("input",		new ComponentInput(),		Input);
	World.Add("form",		new ComponentForm(),		Calculation);
	World.Add("renderer",	new ComponentRenderer(),	Output);

	World.Init();

	string Message = World.UpdateWhile();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}