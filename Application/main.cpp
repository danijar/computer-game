#pragma once

#include "system.h"

#include <string>
using namespace std;

#include "settings/settings.cpp"
#include "window/window.cpp"
#include "input/input.cpp"
#include "model/form.cpp"
#include "model/material.cpp"
#include "model/texture.cpp"
#include "model/mesh.cpp"
#include "physic/transform.cpp"
#include "movement/movement.cpp"
#include "animation/animation.cpp"
#include "camera/camera.cpp"
#include "renderer/shader.cpp"
#include "renderer/framebuffer.cpp"
#include "renderer/pipeline.cpp"
#include "renderer/renderer.cpp"
#include "interface/text.cpp"


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
