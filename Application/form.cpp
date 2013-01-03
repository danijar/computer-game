#pragma once

#include "system.h"

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
			unsigned int id = CreateCube("forms/textures/magic.jpg");
			Entity->Add<StorageMovement>(id);
			Entity->Add<StorageAnimation>(id);
		});

		Event->Listen("SystemInitialized", [=]{
			const int a = 3;

			for(float i = -2; i <= 2;  ++i)
			for(float j = -2; j <= 2; j+=4)
			for(float k = -2; k <= 2;  ++k)
				CreateCube("forms/textures/dirt.jpg", vec3(i*a, j*a, k*a));

			for(float i = -2; i <= 2; i+=2)
			for(float j = -1; j <= 1;  ++j)
			for(float k = -2; k <= 2; k+=2)
				if(i == 0 && k == 0) continue;
				else CreateCube("forms/textures/dirt.jpg", vec3(i*a, j*a, k*a));
		});
	}

	template<size_t PositionsN, size_t TexcoordsN, size_t ElementsN>
	int Create(const float (&Positions)[PositionsN], const float (&Texcoords)[TexcoordsN], const int(&Elements)[ElementsN], string Texture, vec3 Position, vec3 Rotation = vec3(0), vec3 Scale = vec3(1))
	{
		auto shd = Global->Get<StorageShader>("shader");
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);

		glGenBuffers(1, &frm->Positions);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), Positions, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Texcoords), Texcoords, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), Elements, GL_STATIC_DRAW);

		Image image;
		bool result = image.loadFromFile(Texture);
		auto size = image.getSize();
		glGenTextures(1, &frm->Texture);
		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, result ? size.x : 1, result ? size.y : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, result ? image.getPixelsPtr() : nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		frm->Program = shd->Program;
		frm->Scale = Scale;
		tsf->Position = Position;
		tsf->Rotation = Rotation;

		return id;
	}

	unsigned int CreateCube(string Texture, vec3 Position = vec3(0))
	{
		const float Positions[] = {-1.,-1.,1.,1.,-1.,1.,1.,1.,1.,-1.,1.,1.,-1.,1.,1.,1.,1.,1.,1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,-1.,-1.,-1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,1.,-1.,1.,-1.,1.,1.,-1.,-1.,1.,1.,-1.,1.,1.,1.};
		const float Texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
		const int   Elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};

		return Create(Positions, Texcoords, Elements, Texture, Position);
	}
};