#pragma once

#include "system.h"

#include <unordered_map>
#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Keyboard.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;
#include <V8/v8.h>

#include "form.h"
#include "transform.h"
#include "material.h"
#include "mesh.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "light.h"

/*
 * Include the sub classes here.
 *
 * #include "model/mesh.cpp";
 * #include "model/material.cpp";
 * #include "model/texture.cpp";
 */


class ModuleModel : public Module
{
	/*
	 * Store maps of internal module data here. These each map the file path to a struct.
	 * The struct is inspired by the related storage struct, but obviously doesn't include the name.
	 *
	 * unordered_map<string, Mesh> meshes;
	 * unordered_map<string, Material> materials;
	 * unordered_map<string, Texture> textures;
	 */

	/*
	 * Moreover, store an instance of ModelMesh, ModelMaterial and ModelTexture is necessary.
	 * By the way, it might be better to name them just Mesh, Material and Texture.
	 * Otherwise just access static functions from them, if they actually are all static.
	 */

	void Init()
	{
		Opengl->Init();

		Listeners();

		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			auto fms = Entity->Get<StorageForm>();
			return "Forms " + to_string(fms.size());
		};

		Script->Bind("model", jsModel);
		Script->Bind("light", jsLight);

		Debug->Print("start loading the scene...");
		Script->Run("init.js");

		Light(vec3(0.5f, 1.0f, 1.5f), 0.0f, vec3(0.75f, 0.74f, 0.67f), 0.2f, StorageLight::DIRECTIONAL);
	}

	void Update()
	{
		Script->Run("update.js");
	}

	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			// move this into a script
			bool many = Keyboard::isKeyPressed(Keyboard::LShift);
			for(int i = 0; i < (many ? 500 : 1); ++i)
			{
				unsigned int id = Model("PrimitiveQube", "magic.mtl", vec3(0, 4, 0), vec3(0), vec3(1), false);
				Entity->Add<StorageMovement>(id);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3(rand() % 360, rand() % 360, rand() % 360);
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

	unsigned int Light(vec3 Position, float Radius, vec3 Color = vec3(1), float Intensity = 1.f, StorageLight::Shape Type = StorageLight::POINT)
	{
		unsigned int id = Entity->New();
		auto tsf = Entity->Add<StorageTransform>(id);
		auto lgh = Entity->Add<StorageLight>(id);

		tsf->Position = Position;
		lgh->Radius = Radius;
		lgh->Color = Color;
		lgh->Intensity = Intensity;
		lgh->Type = Type;

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

	static v8::Handle<v8::Value> jsModel(const v8::Arguments& args)
	{
		ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

		string mesh = *v8::String::Utf8Value(args[0]);
		string material = *v8::String::Utf8Value(args[1]);
		vec3 position(args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue());
		vec3 rotation(args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue());
		vec3 scale(args[8]->NumberValue());
		bool still = args[8]->BooleanValue();

		unsigned int id = module->Model(mesh, material, position, rotation, scale, still);
		return v8::Uint32::New(id);
	}

	static v8::Handle<v8::Value> jsLight(const v8::Arguments& args)
	{
		ModuleModel* module = (ModuleModel*)HelperScript::Unwrap(args.Data());

		vec3 position(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
		float radius = (float)args[3]->NumberValue();
		vec3 color(args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue());
		float intensity  = (float)args[7]->NumberValue();

		unsigned int id = module->Light(position, radius, color, intensity);
		return v8::Uint32::New(id);
	}
};
