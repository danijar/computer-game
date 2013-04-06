#pragma once

#include "system.h"

#include <string>
using namespace std;

#include "settings/settings.cpp"
#include "window/window.cpp"
#include "input/input.cpp"
#include "model/module.h"
#include "physic/transform.cpp"
#include "movement/movement.cpp"
#include "animation/animation.cpp"
#include "camera/camera.cpp"
#include "renderer/module.h"
#include "interface/text.cpp"


int main()
{
	System World;

	World.Add("settings",    new ModuleSettings());
	World.Add("window",      new ModuleWindow());
	World.Add("input",       new ModuleInput());
	World.Add("model",       new ModuleModel());
	World.Add("transform",   new ModuleTransform());
	World.Add("camera",      new ModuleCamera());
	World.Add("movement",    new ModuleMovement());
	World.Add("animation",   new ModuleAnimation());
	World.Add("renderer",    new ModuleRenderer());
	World.Add("text",        new ModuleText());

	World.Init();

	string Message = World.UpdateLoop();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
