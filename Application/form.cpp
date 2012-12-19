#pragma once

#include "system.h"
#include "debug.h"

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


class ComponentForm : public Component
{
	Clock clock;
	
	void Init()
	{
		Listeners();
	}

	void Update()
	{
		auto fms = Entity->Get<StorageForm>();
		float time = clock.getElapsedTime().asSeconds();

		for(auto i = fms.begin(); i != fms.end(); ++i)
		{
			unsigned int id = i->first;
			auto frm = i->second;

			// update
		}
	}
	
	void Listeners()
	{
		Event->Listen("InputBindCreate", [=]{
			const float VERTICES[] = {-1.f,-1.f,1.f,1.f,0.f,0.f,.8f,1.f,-1.f,1.f,0.f,1.f,0.f,.8f,1.f,1.f,1.f,0.f,0.f,1.f,.8f,-1.f,1.f,1.f,1.f,1.f,1.f,.8f,-1.f,-1.f,-1.f,0.f,0.f,1.f,.8f,1.f,-1.f,-1.f,1.f,1.f,1.f,.8f,1.f,1.f,-1.f,1.f,0.f,0.f,.8f,-1.f,1.f,-1.f,0.f,1.f,0.f,.8f};
			const int   ELEMENTS[] = {0,1,2,2,3,0,1,5,6,6,2,1,7,6,5,5,4,7,4,0,3,3,7,4,4,5,1,1,0,4,3,2,6,6,7,3};
			Create(VERTICES, ELEMENTS);
		});

		Event->Listen("WindowRecreated", [=]{
			auto fms = Entity->Get<StorageForm>();
			for(auto i = fms.begin(); i != fms.end(); ++i)
			{
				glBindBuffer(GL_ARRAY_BUFFER, i->second->VertexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->second->ElementBuffer);
			}
		});
	}

	template<size_t VerticesN, size_t ElementsN>
	void Create(const float (&Vertices)[VerticesN], const int (&Elements)[ElementsN]) // pass [path to model], [path to texture], [position], [rotation] and [scale] instead of using example data
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		Entity->Add<StorageMovement>(id); // if movable
		Entity->Add<StorageAnimation>(id); // if movable

		glGenBuffers(1, &frm->VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, frm->VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &frm->ElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), Elements, GL_STATIC_DRAW);

		frm->Scale = vec3(.25f);
		tsf->Position = vec3(-1.5f, -1.5f, 0);
		tsf->Rotation = vec3(0, 0, 1);

		Debug::Pass("Form cube added");
	}
};