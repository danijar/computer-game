#pragma once

#include "system.h"
#include "debug.h"

#include <string>
using namespace std;

#include "settings.cpp"
#include "window.cpp"
#include "input.cpp"
#include "form.cpp"
#include "material.cpp"
#include "texture.cpp"
#include "mesh.cpp"
#include "transform.cpp"
#include "movement.cpp"
#include "animation.cpp"
#include "camera.cpp"
#include "shader.cpp"
#include "framebuffer.cpp"
#include "pipeline.cpp"
#include "renderer.cpp"
#include "text.cpp"


int main()
{
	System World;

	// maybe get rid of priority numbers, would be a proof for multi thread ability
	World.Add(0, "settings",    new ModuleSettings());
	World.Add(1, "window",      new ModuleWindow());
	World.Add(2, "input",       new ModuleInput());
	World.Add(2, "form",        new ModuleForm());
	World.Add(2, "material",    new ModuleMaterial());
	World.Add(2, "texture",     new ModuleTexture());
	World.Add(2, "mesh",        new ModuleMesh());
	World.Add(2, "transform",   new ModuleTransform());
	World.Add(2, "camera",      new ModuleCamera());
	World.Add(3, "movement",    new ModuleMovement());
	World.Add(3, "animation",   new ModuleAnimation());
	World.Add(3, "shader",      new ModuleShader());
	World.Add(3, "framebuffer", new ModuleFramebuffer());
	World.Add(4, "pipeline",    new ModulePipeline());
	World.Add(5, "renderer",    new ModuleRenderer());
	World.Add(6, "text",        new ModuleText());

	World.Init();

	string Message = World.UpdateLoop();
	cout << "System exited with message: " << Message << endl;

	// cin.get();
}
