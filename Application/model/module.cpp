#include "module.h"

#include <unordered_map>
#include <cstdlib>
#include <GLEW/glew.h>
#include <SFML/Window/Keyboard.hpp>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
#include <V8/v8.h>
using namespace std;
using namespace sf;
using namespace glm;

#include "model.h"
#include "transform.h"
#include "text.h"
#include "light.h"


void ModuleModel::Init()
{
	Opengl->Init();

	Listeners();

	Entity->Add<StorageText>(Entity->New())->Text = [=]{
		auto fms = Entity->Get<StorageModel>();
		return "Forms          " + to_string(fms.size()          ) + '\n'
			 + "Meshes         " + to_string(meshes.size()       ) + '\n'
			 + "Materials      " + to_string(materials.size()    ) + '\n'
			 + "Textures       " + to_string(textures.size()     ) + '\n'
			 + "Static shapes  " + to_string(staticshapes.size() ) + '\n'
			 + "Dynamic shapes " + to_string(dynamicshapes.size());
	};

	Callbacks();

	Light(vec3(0.5f, 1.0f, 1.5f), 0.0f, vec3(0.75f, 0.74f, 0.67f), 0.2f, StorageLight::DIRECTIONAL);
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

	Event->Listen("InputBindCreate", [=]{

		// move this into a script
		if(Keyboard::isKeyPressed(Keyboard::LShift))
		{
			for(int i = 0; i < 20; ++i)
			{
				unsigned int id = Model("qube.prim", "magic.mtl", vec3(0, 10, 0), vec3(0), vec3(1), 4.0f);
				auto tsf = Entity->Get<StorageTransform>(id);

				tsf->Rotation(vec3(rand() % 360, rand() % 360, rand() % 360));
				tsf->Position(vec3(0, 50, 0));
				tsf->Body->applyCentralImpulse(btVector3((rand() % 200) / 10.0f - 10.0f, -500.0f, (rand() % 200) / 10.0f - 10.0f));
			}
		}
		else
		{
			unsigned int id = Model("qube.prim", "magic.mtl", vec3(0, 10, 0), vec3(0), vec3(1), 4.0f);
			Entity->Get<StorageTransform>(id)->Rotation(vec3(rand() % 360, rand() % 360, rand() % 360));
		}
	});

	Event->Listen("InputBindShoot", [=]{

		// move this into a script
		unsigned int id = Model("qube.prim", "magic.mtl", vec3(0, 10, 0), vec3(0), vec3(1), 10.0f);
		auto tsfcam = Entity->Get<StorageTransform>(*Global->Get<unsigned int>("camera"));
		auto tsfcbe = Entity->Get<StorageTransform>(id);

		vec3 lookat = tsfcam->Direction();
		
		tsfcbe->Rotation(vec3(rand() % 360, rand() % 360, rand() % 360));
		tsfcbe->Position(tsfcam->Position() + 1.5f * lookat);
		tsfcbe->Body->applyCentralImpulse(1000.0f * btVector3(lookat.x, lookat.y, lookat.z));
	});
}

unsigned int ModuleModel::Model(string Mesh, string Material, vec3 Position, vec3 Rotation , vec3 Scale, float Mass)
{
	unsigned int id = Entity->New();
	auto frm = Entity->Add<StorageModel>(id);
	auto tsf = Entity->Add<StorageTransform>(id);

	ModuleModel::Mesh mesh = GetMesh(Mesh);
	frm->Positions = mesh.Positions;
	frm->Normals   = mesh.Normals;
	frm->Texcoords = mesh.Texcoords;
	frm->Elements  = mesh.Elements;

	ModuleModel::Material material = GetMaterial(Material);
	frm->Diffuse     = GetTexture(material.Diffuse);
	// frm->Normal   = textures.Get(material.Normal);
	// frm->Specular = textures.Get(material.Specular);

	delete tsf->Body;
	tsf->Body = CreateBody(Mesh, Mass);
	tsf->Position(Position);
	tsf->Rotation(Rotation);
	tsf->Scale(Scale);

	return id;
}

unsigned int ModuleModel::Light(vec3 Position, float Radius, vec3 Color, float Intensity, StorageLight::Shape Type)
{
	unsigned int id = Entity->New();
	auto tsf = Entity->Add<StorageTransform>(id);
	auto lgh = Entity->Add<StorageLight>(id);

	tsf->Position(Position);
	lgh->Radius = Radius;
	lgh->Color = Color;
	lgh->Intensity = Intensity;
	lgh->Type = Type;

	return id;
}
