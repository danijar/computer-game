#pragma once

#include "system.h"
#include "debug.h"

#include "settings.cpp"
#include "window.cpp"
#include "input.cpp"
#include "form.cpp"
#include "movement.cpp"
#include "animation.cpp"
#include "camera.cpp"
#include "rendererforward.cpp"
#include "rendererdeferred.cpp"
#include "text.cpp"


int main()
{
	System World;

	World.Add(0, "settings",  new ComponentSettings());
	World.Add(1, "window",    new ComponentWindow());
	World.Add(1, "input",     new ComponentInput());
	World.Add(2, "form",      new ComponentForm());
	World.Add(2, "movement",  new ComponentMovement());
	World.Add(2, "animation", new ComponentAnimation());
	World.Add(2, "camera",    new ComponentCamera());
	//World.Add(3, "renderer",  new ComponentRendererForward());
	World.Add(3, "renderer",  new ComponentRendererDeferred());
	//World.Add(3, "text",      new ComponentText());

	World.Init();

	string Message = World.UpdateWhile();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
