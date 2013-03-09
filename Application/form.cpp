#pragma once

#include "system.h"

#include <unordered_map>
#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "form.h"
#include "transform.h"
#include "movement.h"
#include "animation.h"
#include "text.h"
#include "keyboard.h"


class ComponentForm : public Component
{
	unordered_map<string, GLuint> textures;
	
	void Init()
	{
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
			int number = KeyDown(Key::LShift) ? 500 : 1;
			for(int i = 0; i < number; ++i)
			{
				unsigned int id = CreateCube("forms/textures/magic.jpg", vec3(0, 4, 0), true);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation = vec3(random(), random(), random());
			}
		});

		Event->Listen("SystemInitialized", [=]{

		});
	}

	int Create(const GLfloat* Vertices, int VerticesN, const GLfloat* Normals, int NormalsN, const GLfloat* Texcoords, int TexcoordsN, const GLuint* Elements, int ElementsN, string Texture, vec3 Position = vec3(0), vec3 Rotation = vec3(0), vec3 Scale = vec3(1), bool Movable = false)
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

		auto i = textures.find(Texture);
		if(i == textures.end())
		{
			Image image;
			bool result = image.loadFromFile(Texture);
			auto size = image.getSize();
			glGenTextures(1, &frm->Texture);
			glBindTexture(GL_TEXTURE_2D, frm->Texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result ? size.x : 1, result ? size.y : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, result ? image.getPixelsPtr() : nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
			textures.insert(make_pair(Texture, frm->Texture));
		}
		else frm->Texture = i->second;

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