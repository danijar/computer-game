#pragma once

#include "system.h"
#include "debug.h"

#include "settings.cpp"
#include "window.cpp"
#include "input.cpp"
#include "form.cpp"
#include "terrain.cpp"
#include "movement.cpp"
#include "animation.cpp"
#include "camera.cpp"
#include "renderer.cpp"
#include "text.cpp"


int main()
{
	System World;

	World.Add("settings",  new ComponentSettings(),  Input);
	World.Add("window",    new ComponentWindow(),    Input);
	World.Add("input",     new ComponentInput(),     Input);
	World.Add("form",      new ComponentForm(),      Calculation);
	World.Add("terrain",	new ComponentTerrain(),		Calculation);
	World.Add("movement",  new ComponentMovement(),  Calculation);
	World.Add("animation", new ComponentAnimation(), Calculation);
	World.Add("camera",    new ComponentCamera(),    Output);
	World.Add("renderer",  new ComponentRenderer(),  Output);
	World.Add("text",      new ComponentText(),      Output);

	World.Init();

	string Message = World.UpdateWhile();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}