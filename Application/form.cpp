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
#include "chunk.h"


class ComponentForm : public Component
{
	void Init()
	{
		//Storage->Add<StorageForms>("forms");
		Storage->Add<StorageForm>("cube");
		Storage->Add<StorageForm>("terrain");

		Listeners();
	}

	void Update()
	{
		/*
		auto fms = Storage->Get<StorageForms>("forms");
		float time = clock.getElapsedTime().asSeconds();

		for(auto i = fms->List.begin(); i != fms->List.end(); ++i)
		{
			// update
		}
		*/

		auto terrain = Storage->Get<StorageForm>("terrain");
		auto chunk = Storage->Get<StorageChunk>("chunk");

		if(chunk->changed)
		{
			chunk->changed = false;
			// update VBO
		}
	}

	Clock clock;

	void Listeners()
	{
		Event->Listen("SystemInited", [=]{
			CreateCube();
		});
		
	}

	void CreateCube() // pass [path to model], [path to texture], [position], [rotation] and [scale] instead of using example data
	{
		//auto fms = Storage->Get<StorageForms>("forms");
		auto cube = Storage->Get<StorageForm>("cube");

		StorageForm frm;

		const float Vertices[] = {
  			-1.f, -1.f,  1.f,  1.f,  0.f,  0.f,  .8f,
			 1.f, -1.f,  1.f,  0.f,  1.f,  0.f,  .8f,
			 1.f,  1.f,  1.f,  0.f,  0.f,  1.f,  .8f,
			-1.f,  1.f,  1.f,  1.f,  1.f,  1.f,  .8f,
   
			-1.f, -1.f, -1.f,  0.f,  0.f,  1.f,  .8f,
			 1.f, -1.f, -1.f,  1.f,  1.f,  1.f,  .8f,
			 1.f,  1.f, -1.f,  1.f,  0.f,  0.f,  .8f,
			-1.f,  1.f, -1.f,  0.f,  1.f,  0.f,  .8f,
		};

		glGenBuffers(1, &frm.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, frm.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
		
		const int Elements[] = {
			0, 1, 2, 2, 3, 0,
			1, 5, 6, 6, 2, 1,
			7, 6, 5, 5, 4, 7,
			4, 0, 3, 3, 7, 4,
			4, 5, 1, 1, 0, 4,
			3, 2, 6, 6, 7, 3,
		};

		glGenBuffers(1, &frm.ElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm.ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Elements), &Elements, GL_STATIC_DRAW);

		frm.Scale = vec3(.25f);
		frm.Position = vec3(-1.5f, -1.5f, 0);
		frm.Rotation = vec3(0, 0, 1);

		//fms->List.push_back(frm);
		*cube = frm;

		Debug::Pass("Form cube added");
	}
};