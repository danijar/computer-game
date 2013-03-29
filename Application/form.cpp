#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"
#include "keyboard.h"

#include <unordered_map>
#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "form.h"
#include "transform.h"
#include "material.h"
#include "mesh.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "light.h"


class ModuleForm : public Module
{
	void Init()
	{
		Opengl::InitGlew();

		// init the scene, this will be done in an initialization script later on
		// or will be loaded from a scene file or database
		const int a = 3;
		for(float x = -3; x <= 3; ++x)
		for(float z = -3; z <= 3; ++z)
			Create("PrimitiveQube", "dirt.mtl", vec3(a*x, 1, a*z));
		for(float x = -3; x <= 3; x+=3)
		for(float z = -3; z <= 3; z+=3)
			if(x == 0 && z == 0) continue;
			else Create("PrimitiveQube", "grass.mtl", vec3(a*x, a+1, a*z));
		Create("PrimitivePlane", "chess.mtl");

		Create("barrel.3ds", "barrel.mtl", vec3(16, 0, 8), vec3(-90, 0, 0), vec3(4));
		Create("shrine.3ds", "shrine.mtl", vec3(40, 0, -10), vec3(-90, 0, -30), vec3(2.5));
		Create("rock.obj", "rock.mtl", vec3(50, 2, 45), vec3(-90, 0, 0), vec3(3.0));

		auto lgt1 = Entity->Add<StorageLight>(Entity->New());
		lgt1->Position = vec3(0, 5, 0);
		lgt1->Radius = 20.f;
		lgt1->Color = vec3(0, 1, 0);

		auto lgt2 = Entity->Add<StorageLight>(Entity->New());
		lgt2->Position = vec3(-10, 0.5, -15);
		lgt2->Radius = 5.f;
		lgt2->Color = vec3(1, 0, 0);
		lgt2->Intensity = 2.0f;

		auto lgt3 = Entity->Add<StorageLight>(Entity->New());
		lgt3->Position = vec3(50, 5, 50);
		lgt3->Radius = 25.f;
		lgt3->Color = vec3(0, 0, 1);

		auto lgt4 = Entity->Add<StorageLight>(Entity->New());
		lgt4->Position = vec3(40, 10, -10);
		lgt4->Radius = 100.f;
		lgt4->Color = vec3(1);
		lgt4->Intensity = 2.5f;

		auto lgt5 = Entity->Add<StorageLight>(Entity->New());
		lgt5->Position = vec3(0, 50, 0);
		lgt5->Radius = 500.f;
		lgt5->Color = vec3(1);
		lgt5->Intensity = 1.0f;

		Listeners();

		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			auto fms = Entity->Get<StorageForm>();
			return "Forms " + to_string(fms.size());
		};
	}

	void Update()
	{

	}

	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			// move this into a script
			int number = KeyDown(Key::LShift) ? 500 : 1;
			for(int i = 0; i < number; ++i)
			{
				unsigned int id = Create("PrimitiveQube", "magic.mtl", vec3(0, 4, 0), vec3(0), vec3(1), false);
				Entity->Add<StorageMovement>(id);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3((rand() % (360 * 10)) / 10.f, (rand() % (360 * 10)) / 10.f, (rand() % (360 * 10)) / 10.f);
			}
		});
	}

	unsigned int Create(string Mesh, string Material, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Static = true)
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);

		frm->Mesh     = this->Mesh(Mesh);
		frm->Material = this->Material(Material);
		tsf->Position = Position;
		tsf->Rotation = Rotation;
		tsf->Scale    = Scale;
		tsf->Static   = Static;

		return id;
	}

	unsigned int Mesh(string Path)
	{
		auto mes = Entity->Get<StorageMesh>();
		for(auto i : mes)
			if(i.second->Path == Path)
				return i.first;

		unsigned int id = Entity->New();
		auto msh = Entity->Add<StorageMesh>(id);
		msh->Path = Path;
		return id;
	}

	unsigned int Material(string Path)
	{
		auto mts = Entity->Get<StorageMaterial>();
		for(auto i : mts)
			if(i.second->Path == Path)
				return i.first;

		unsigned int id = Entity->New();
		auto mat = Entity->Add<StorageMaterial>(id);
		mat->Path = Path;
		return id;
	}
};
