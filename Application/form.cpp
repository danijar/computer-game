#pragma once

#include "system.h"

#include <unordered_map>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Image.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "form.h"
#include "transform.h"
#include "movement.h"
#include "animation.h"
#include "shader.h"
#include "text.h"


class ComponentForm : public Component
{
	Clock clock;
	unordered_map<string, GLuint> textures;
	
	void Init()
	{
		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			auto fms = Entity->Get<StorageForm>();
			return "Forms " + to_string(fms.size());
		};

		Listeners();
	}

	void Update()
	{

	}
	
	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			int number = KeyDown(Key::LShift) ? 500 : 1;
			for(int i = 0; i < number; ++i)
			{
				unsigned int id = CreateCube("forms/textures/magic.jpg", vec3(0, 4, 0));
				Entity->Add<StorageMovement>(id);
				Entity->Add<StorageAnimation>(id);
				auto tsf = Entity->Get<StorageTransform>(id);
				tsf->Rotation += vec3(.5, 0, .5);
			}
		});

		Event->Listen("SystemInitialized", [=]{

		});
	}

	int Create(const float* Vertices, int VerticesN, const float* Normals, int NormalsN, const float* Texcoords, int TexcoordsN, const int* Elements, int ElementsN, string Texture, vec3 Position, vec3 Rotation = vec3(0), vec3 Scale = vec3(1))
	{
		auto shd = Global->Get<StorageShader>("shader");
		unsigned int id = Entity->New();

		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);

		glGenBuffers(1, &frm->Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
		glBufferData(GL_ARRAY_BUFFER, VerticesN * sizeof(float), Vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, NormalsN * sizeof(float), Normals, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, TexcoordsN * sizeof(float), Texcoords, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementsN * sizeof(int), Elements, GL_STATIC_DRAW);

		auto i = textures.find(Texture);
		if(i == textures.end())
		{
			Image image;
			bool result = image.loadFromFile(Texture);
			auto size = image.getSize();
			glGenTextures(1, &frm->Texture);
			glBindTexture(GL_TEXTURE_2D, frm->Texture);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, result ? size.x : 1, result ? size.y : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, result ? image.getPixelsPtr() : nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glGenerateMipmap(GL_TEXTURE_2D);
			textures.insert(make_pair(Texture, frm->Texture));
		}
		else frm->Texture = i->second;

		frm->Program = shd->Program;
		frm->Scale = Scale;
		tsf->Position = Position;
		tsf->Rotation = Rotation;

		return id;
	}

	unsigned int CreateCube(string Texture, vec3 Position = vec3(0))
	{
		const float Vertices[] = {-1.,-1.,1.,1.,-1.,1.,1.,1.,1.,-1.,1.,1.,-1.,1.,1.,1.,1.,1.,1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,-1.,-1.,-1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,1.,-1.,1.,-1.,1.,1.,-1.,-1.,1.,1.,-1.,1.,1.,1.};
		const float Normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
		const float Texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
		const int   Elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};

		return Create(Vertices, 72, Normals, 72, Texcoords, 48, Elements, 36, Texture, Position);
	}

	typedef Keyboard::Key Key;
	bool KeyDown(Keyboard::Key key) { return Keyboard::isKeyPressed(key); }
};