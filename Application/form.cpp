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
#include "texture.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "keyboard.h"

class ModuleForm : public Module
{
	void Init()
	{
		Opengl::InitGlew();

		Listeners();

		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			auto fms = Entity->Get<StorageForm>();
			return "Forms " + to_string(fms.size());
		};

		Load("barrel.3ds", vec3(16, 5.8, 8), vec3(90, 0, 0), vec3(5));
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
				unsigned int id = CreateCube("magic.jpg", vec3(0, 4, 0), true);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3(random(), random(), random());
			}
		});

		Event->Listen("SystemInitialized", [=]{
			// move this into the script init.js and run that from here
			const int a = 3;
			for(float x = -3; x <= 3; ++x)
			for(float z = -3; z <= 3; ++z)
				CreateCube("dirt.jpg", vec3(a*x, 1, a*z));
			for(float x = -3; x <= 3; x+=3)
			for(float z = -3; z <= 3; z+=3)
				if(x == 0 && z == 0) continue;
				else CreateCube("grass.jpg", vec3(a*x, a+1, a*z));
			CreatePlane("bottom.jpg", 20);
		});
	}

	// creation

	unsigned int Create(const GLfloat* Vertices, int VerticesN, const GLfloat* Normals, int NormalsN, const GLfloat* Texcoords, int TexcoordsN, const GLuint* Elements, int ElementsN, string Texture, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Movable = false)
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		if(Movable) Entity->Add<StorageMovement>(id);

		glGenBuffers(1, &frm->Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
		glBufferData(GL_ARRAY_BUFFER, VerticesN * sizeof(GLfloat), Vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, NormalsN * sizeof(GLfloat), Normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, TexcoordsN * sizeof(GLfloat), Texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementsN * sizeof(GLuint), Elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		auto txs = Entity->Get<StorageTexture>();
		for(auto i : txs)
		{
			if(i.second->Path == Texture)
			{
				frm->Texture = i.second->Id;
				break;
			}
		}
		if(!frm->Texture)
		{
			unsigned int id = Entity->New();
			auto tex = Entity->Add<StorageTexture>(id);
			tex->Path = Texture;
			frm->Texture = tex->Id;
		}

		tsf->Position = Position;
		tsf->Rotation = Rotation;
		tsf->Scale = Scale;
		Matrix(id);

		return id;
	}

	unsigned int CreateCube(string Texture, vec3 Position, bool Movable = false)
	{
		const GLfloat Vertices[]  = {-1.,-1.,1.,1.,-1.,1.,1.,1.,1.,-1.,1.,1.,-1.,1.,1.,1.,1.,1.,1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,-1.,-1.,-1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,1.,-1.,1.,-1.,1.,1.,-1.,-1.,1.,1.,-1.,1.,1.,1.};
		const GLfloat Normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
		const GLfloat Texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
		const GLuint  Elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};

		return Create(Vertices, 72, Normals, 72, Texcoords, 48, Elements, 36, Texture, Position, vec3(0), vec3(1), Movable);
	}

	unsigned int CreatePlane(string Texture, float length, vec3 Position = vec3(0))
	{
		const float l = length;
		const GLfloat Vertices[]  = { -l,0,-l, -l,0,l, l,0,l, l,0,-l };
		const GLfloat Normals[]   = { 0,1,0, 0,1,0., 0,1,0, 0,1,0 };
		const GLfloat Texcoords[] = { 0,0, l/2,0, l/2,l/2, 0,l/2 };
		const GLuint  Elements[]  = { 0,1,2, 2,3,0 };
		return Create(Vertices, 12, Normals, 12, Texcoords, 8, Elements, 6, Texture, Position);
	}

	// model loading

	unsigned int Load(string Path, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Movable = false)
	{
		Lib3dsFile* model = lib3ds_file_open((Name() + "/" + Path).c_str());
		if(model == false)
		{
			Debug::Fail("Form loading of (" + Path + ") failed.");
			return 0;
		}

		vector<GLfloat> vertices, normals, texcoords;
		vector<GLuint> elements;

		GLuint element = 0;
		for (int i = 0; i < model->nmeshes; ++i)
		{
			Lib3dsMesh *mesh = model->meshes[i];

			for(unsigned int j = 0; j < mesh->nfaces; ++j)
			{
				Lib3dsFace *face = &mesh->faces[j];
				for(int k = 0; k < 3; ++k)
				{
					vertices.push_back(mesh->vertices[face->index[k]][0]);
					vertices.push_back(mesh->vertices[face->index[k]][1]);
					vertices.push_back(mesh->vertices[face->index[k]][2]);

					texcoords.push_back(mesh->texcos[face->index[k]][0]);
					texcoords.push_back(mesh->texcos[face->index[k]][1]);

					elements.push_back(element++);
				}
			}

			int last = normals.size();
			normals.resize(vertices.size());
			lib3ds_mesh_calculate_vertex_normals(mesh, (float(*)[3])(&normals[last]));
		}

		string texture = string(model->materials[0]->texture1_map.name);
		texture = texture.substr(0, texture.size() - 1) + ".jpg"; // remove the last character because whyever it's a dot

		Debug::Pass("Loaded (" + Path + ") with " + to_string(vertices.size()) + " vertices, " + to_string(normals.size()) + " normals, " + to_string(texcoords.size()) + " texcoords and texture (" + texture + ").");
		
		return Create(&vertices[0], vertices.size(), &normals[0], normals.size(), &texcoords[0], texcoords.size(), &elements[0], elements.size(), texture, Position, Rotation, Scale, Movable);
	}

	// helpers

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

	inline float random(int precision = 10){ return (float)(rand() % (360 * precision)) / (float)precision; }
};
