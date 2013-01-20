#pragma once

#include "system.h"

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


class ComponentForm : public Component
{
	Clock clock;
	
	void Init()
	{
		Listeners();
	}

	void Update()
	{

	}
	
	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			unsigned int id = CreateCube("forms/textures/magic.jpg", vec3(0, 4, 0));
			Entity->Add<StorageMovement>(id);
			Entity->Add<StorageAnimation>(id);
			auto tsf = Entity->Get<StorageTransform>(id);
			tsf->Rotation += vec3(.5, 0, .5);
		});

		Event->Listen("SystemInitialized", [=]{
			/*
			const int a = 3;
			for(float x = -3; x <= 3; ++x)
			for(float z = -3; z <= 3; ++z)
				CreateCube("forms/textures/dirt.jpg", vec3(a*x, 1, a*z));

			for(float x = -3; x <= 3; x+=3)
			for(float z = -3; z <= 3; z+=3)
				if(x == 0 && z == 0) continue;
				else CreateCube("forms/textures/grass.jpg", vec3(a*x, a+1, a*z));

			const float l = 30.f;
			const float Vertices[] = { -l,0,-l, -l,0,l, l,0,l, l,0,-l };
			const float Normals[]   = { 0,1,0, 0,1,0., 0,1,0, 0,1,0 };
			const float Texcoords[] = { 0,0, l/2,0, l/2,l/2, 0,l/2 };
			const int   Elements[]  = { 0,1,2, 2,3,0 };
			Create(Vertices, 12, Normals, 12, Texcoords, 8, Elements, 6, "forms/textures/bottom.jpg", vec3(0));
			*/
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
};