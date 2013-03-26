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
#include <LIB3DS/lib3ds.h>

#include "form.h"
#include "transform.h"
#include "material.h"
#include "mesh.h"
#include "movement.h"
#include "animation.h"
#include "text.h"


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
