#pragma once

#include "system.h"
#include "debug.h"

#include <cstdlib>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Keyboard.hpp>
using namespace sf;
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "form.h"


class ComponentForm : public Component
{
	void Init()
	{
		auto fms = Storage->Add<StorageForms>("forms");
		auto mvs = Storage->Add<StorageMovables>("movables");

		Listeners();
	}

	void Update()
	{
		auto fms = Storage->Get<StorageForms>("forms");
		auto mvs = Storage->Get<StorageMovables>("movables");
		float time = clock.getElapsedTime().asSeconds();

		vec3 move;
		const float spd = .03f;
		if (Keyboard::isKeyPressed(Keyboard::Up)   ) move += vec3(-spd, -spd, 0);
		if (Keyboard::isKeyPressed(Keyboard::Down) ) move += vec3( spd,  spd, 0);
		if (Keyboard::isKeyPressed(Keyboard::Left) ) move += vec3( spd, -spd, 0);
		if (Keyboard::isKeyPressed(Keyboard::Right)) move += vec3(-spd,  spd, 0);
		for(auto i = fms->List.begin(); i != fms->List.end(); ++i)
		{
			i->Position += move;
		}
		
	}

	Clock clock;

	void Listeners()
	{
		Event->Listen("SystemInited", [=]{
			Create();
		});
		
	}

	void Create() // pass [path to model], [path to texture], [position], [rotation] and [scale] instead of using example data
	{
		auto fms = Storage->Get<StorageForms>("forms");

		Form frm;

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

		fms->List.push_back(frm);

		Debug::Pass("Form added");
	}
};