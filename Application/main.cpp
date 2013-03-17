#pragma once

#include "system.h"
#include "debug.h"

#include <string>
using namespace std;

#include "settings.cpp"
#include "window.cpp"
#include "input.cpp"
#include "form.cpp"
#include "texture.cpp"
#include "movement.cpp"
#include "animation.cpp"
#include "camera.cpp"
#include "rendererforward.cpp"
#include "rendererdeferred.cpp"
#include "text.cpp"


int main()
{
	System World;

	World.Add(0, "settings",  new ModuleSettings());
	World.Add(1, "window",    new ModuleWindow());
	World.Add(1, "input",     new ModuleInput());
	World.Add(2, "form",      new ModuleForm());
	World.Add(2, "texture",   new ModuleTexture());
	World.Add(2, "movement",  new ModuleMovement());
	World.Add(2, "animation", new ModuleAnimation());
	World.Add(2, "camera",    new ModuleCamera());
	//World.Add(3, "renderer",  new ModuleRendererForward());
	World.Add(3, "renderer",  new ModuleRendererDeferred());
	World.Add(4, "text",      new ModuleText());

	World.Init();

	string Message = World.UpdateLoop();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
