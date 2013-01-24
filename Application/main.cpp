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
#include "shader.cpp"
#include "renderer.cpp"
#include "text.cpp"


int main()
{
	System World;

	World.Add(0, "settings",  new ComponentSettings());
	World.Add(1, "window",    new ComponentWindow());
	World.Add(1, "input",     new ComponentInput());
	World.Add(2, "shader",    new ComponentShader());
	World.Add(3, "form",      new ComponentForm());
	World.Add(3, "terrain",   new ComponentTerrain());
	World.Add(3, "movement",  new ComponentMovement());
	World.Add(3, "animation", new ComponentAnimation());
	World.Add(3, "camera",    new ComponentCamera());
	World.Add(4, "renderer",  new ComponentRenderer());
	World.Add(4, "text",      new ComponentText());

	World.Init();

	string Message = World.UpdateWhile();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}