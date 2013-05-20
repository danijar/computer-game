#include "module.h"

#include <unordered_map>
#include <cstdlib>
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace sf;
using namespace glm;

#include "model.h"
#include "form.h"
#include "print.h"
#include "light.h"


void ModuleModel::Init()
{
	Opengl->Init();

	Listeners();
	Callbacks();

	Entity->Add<Print>(Entity->New())->Text = [=]{
		auto fms = Entity->Get<Model>();
		return "Forms     " + to_string(fms.size()      ) + '\n'
			 + "Meshes    " + to_string(meshes.size()   ) + '\n'
			 + "Materials " + to_string(materials.size()) + '\n'
			 + "Textures  " + to_string(textures.size() ) + '\n'
			 + "Shapes    " + to_string(shapes.size()   );
	};

	// CreateLight(vec3(0.5f, 1.0f, 1.5f), 0.0f, vec3(0.75f, 0.74f, 0.67f), 0.7f, Light::DIRECTIONAL);
	Script->Run("init.js");
}

void ModuleModel::Update()
{
	Script->Run("update.js");
}

void ModuleModel::Listeners()
{
	Event->Listen("WindowFocusGained", [=]{
		ReloadMeshes();
		ReloadMaterials();
		ReloadTextures();
	});

	auto cook = [](float from, float to){ return (rand() % (int)((to - from) * 10) / 10.f) + from; };

	// move this into a script
	Event->Listen("InputBindShoot", [=]{
		unsigned int id = CreateModel("qube.prim", "magic.mtl", vec3(0, 10, 0), vec3(0), vec3(0.5f), 5.0f);
		auto tsfcam = Entity->Get<Form>(*Global->Get<unsigned int>("camera"));
		auto tsfcbe = Entity->Get<Form>(id);

		vec3 lookat = tsfcam->Direction();
		
		tsfcbe->Rotation(vec3(cook(0.0f, 6.28f), cook(0.0f, 6.28f), cook(0.0f, 6.28f)));
		tsfcbe->Position(tsfcam->Position() + lookat);
		tsfcbe->Body->applyCentralImpulse(200.0f * btVector3(lookat.x, lookat.y, lookat.z));
	});
}
