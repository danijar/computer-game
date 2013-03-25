#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"

#include <unordered_map>
#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;
#include <LIB3DS/lib3ds.h>

#include "form.h"
#include "transform.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "keyboard.h"
#include "material.h"
#include "mesh.h"


class ModuleForm : public Module
{
	void Init()
	{
		Opengl::InitGlew();
		
		// init the scene, this will be done in an initialization script later on
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
		auto mvs = Entity->Get<StorageMovement>();

		for(auto i : mvs)
		{
			Matrix(i.first);
		}
	}
	
	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			// move this into a script
			int number = KeyDown(Key::LShift) ? 500 : 1;
			for(int i = 0; i < number; ++i)
			{
				unsigned int id = Create("PrimitiveQube", "magic.mtl", vec3(0, 4, 0), vec3(0), vec3(1), true);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3(random(), random(), random());
			}
		});
	}

	unsigned int Create(string Mesh, string Material, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Movable = false)
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		if(Movable) Entity->Add<StorageMovement>(id);

		frm->Mesh     = this->Mesh(Mesh);
		frm->Material = this->Material(Material);
		tsf->Position = Position;
		tsf->Rotation = Rotation;
		tsf->Scale = Scale;
		Matrix(id);

		return id;
	}

	void Matrix(unsigned int id)
	{
		auto tsf = Entity->Get<StorageTransform>(id);

		mat4 Scale      = scale    (mat4(1), tsf->Scale);
		mat4 Translate  = translate(mat4(1), tsf->Position);
		mat4 Rotate     = rotate   (mat4(1), tsf->Rotation.x, vec3(1, 0 ,0))
						* rotate   (mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
						* rotate   (mat4(1), tsf->Rotation.z, vec3(0, 0, 1));
		tsf->Matrix = Translate * Rotate * Scale;
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

	inline vector<GLfloat> vec(const GLfloat array[], uint length) { return vector<GLfloat>(array, array + length); }
	inline vector<GLuint> vec(const GLuint array[], uint length) { return vector<GLuint>(array, array + length); }

	inline float random(int precision = 10){ return (float)(rand() % (360 * precision)) / (float)precision; }
};
