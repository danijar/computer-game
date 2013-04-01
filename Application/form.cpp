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
			Model("PrimitiveQube", "dirt.mtl", vec3(a*x, 1, a*z));
		for(float x = -3; x <= 3; x+=3)
		for(float z = -3; z <= 3; z+=3)
			if(x == 0 && z == 0) continue;
			else Model("PrimitiveQube", "grass.mtl", vec3(a*x, a+1, a*z));
		Model("PrimitivePlane", "chess.mtl");

		Model("barrel.3ds", "barrel.mtl", vec3(16, 0, 8), vec3(-90, 0, 0), vec3(4));
		Model("shrine.3ds", "shrine.mtl", vec3(40, 0, -10), vec3(-90, 0, -30), vec3(2.5));
		Model("rock.obj", "rock.mtl", vec3(50, 2, 45), vec3(-90, 0, 0), vec3(5.0));

		Light(vec3(0, 5, 0), 20.f, vec3(0, 1, 0));
		Light(vec3(-10, 0.5, -15), 7.f, vec3(1, 0, 0), 2.f);
		Light(vec3(50, 5, 50), 25.f, vec3(0, 0, 1));
		Light(vec3(40, 10, -10), 100.f, vec3(1), 2.5f);
		//Light(vec3(0, 50, 50), 500.f, vec3(1)); // pointlight for daylight

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
			bool many = KeyDown(Key::LShift);
			for(int i = 0; i < (many ? 500 : 1); ++i)
			{
				unsigned int id = Model("PrimitiveQube", "magic.mtl", vec3(0, 4, 0), vec3(0), vec3(1), false);
				Entity->Add<StorageMovement>(id);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3(rand() % 360, rand() % 360, rand() % 360);
			}
			for(int i = 0; i < (many ? 20 : 1); ++i)
			{
				vec3 position = vec3((rand() % 50) - 25, 5, (rand() % 50) - 25);
				float radius = (rand() % 30) + 10.f;
				vec3 color = vec3((rand() % 100)/100.f, (rand() % 100)/100.f, (rand() % 100)/100.f);
				unsigned int id = Light(position, radius, color);
				Entity->Add<StorageMovement>(id)->Type = StorageMovement::PLANAR;
			}
		});
	}

	unsigned int Model(string Mesh, string Material, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Static = true)
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

	unsigned int Light(vec3 Position, float Radius, vec3 Color = vec3(1), float Intensity = 1.f)
	{
		unsigned int id = Entity->New();
		auto tsf = Entity->Add<StorageTransform>(id);
		auto lgh = Entity->Add<StorageLight>(id);

		tsf->Position = Position;
		lgh->Radius = Radius;
		lgh->Color = Color;
		lgh->Intensity = Intensity;

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
