#pragma once

#include "system.h"

using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
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
			unsigned int id = CreateCube(vec3(0));
			Entity->Add<StorageMovement>(id);
			Entity->Add<StorageAnimation>(id);
		});

		Event->Listen("SystemInitialized", [=]{
			const int a = 3;

			for(float i = -2; i <= 2;  ++i)
			for(float j = -2; j <= 2; j+=4)
			for(float k = -2; k <= 2;  ++k)
				CreateCube(vec3(i*a, j*a, k*a));

			for(float i = -2; i <= 2; i+=2)
			for(float j = -1; j <= 1;  ++j)
			for(float k = -2; k <= 2; k+=2)
				if(i == 0 && k == 0) continue;
				else CreateCube(vec3(i*a, j*a, k*a));
		});
	}

	template<size_t VerticesPositionN, size_t VerticesColorN, size_t ElementsN>
	int Create(const float (&VerticesPosition)[VerticesPositionN], const float (&VerticesColor)[VerticesColorN], const int (&Elements)[ElementsN], vec3 Position, vec3 Rotation = vec3(0), vec3 Scale = vec3(1))
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		auto shd = Global->Get<StorageShader>("shader");

		frm->Program = shd->Program;

		glGenBuffers(1, &frm->VerticesPosition);
		glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesPosition);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesPosition), VerticesPosition, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->VerticesColor);
		glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesColor), VerticesColor, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), Elements, GL_STATIC_DRAW);

		frm->Scale = Scale;
		tsf->Position = Position;
		tsf->Rotation = Rotation;

		return id;
	}

	unsigned int CreateCube(vec3 Position)
	{
		const float POSITIONS[] = {-1.f,-1.f,+1.f,+1.f,-1.f,+1.f,+1.f,+1.f,+1.f,-1.f,+1.f,+1.f,-1.f,-1.f,-1.f,+1.f,-1.f,-1.f,+1.f,+1.f,-1.f,-1.f,+1.f,-1.f};
		const float COLORS[]    = {1.f,0.f,0.f,.8f,0.f,1.f,0.f,.8f,0.f,0.f,1.f,.8f,1.f,1.f,1.f,.8f,0.f,0.f,1.f,.8f,1.f,1.f,1.f,.8f,1.f,0.f,0.f,.8f,0.f,1.f,0.f,.8f};
		const int   ELEMENTS[]  = {0,1,2,2,3,0,1,5,6,6,2,1,7,6,5,5,4,7,4,0,3,3,7,4,4,5,1,1,0,4,3,2,6,6,7,3};
		unsigned int id = Create(POSITIONS, COLORS, ELEMENTS, Position);
		return id;
	}
};