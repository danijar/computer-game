#pragma once

#include "system.h"
#include "debug.h"

#include <stdio.h>
#include <vector>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <glm/gtc/noise.hpp>
using namespace glm;

#include "form.h"
#include "transform.h"
#include "chunk.h"
#include "shader.h"
#include "movement.h"


class ComponentTerrain : public Component
{
	unsigned int id;

	// testing
	int wait;

	void Init()
	{
		id = Entity->New();
		Entity->Add<StorageChunk>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		auto frm = Entity->Add<StorageForm>(id);
		Entity->Add<StorageMovement>(id);

		tsf->Position = vec3(-5);
		frm->Scale = vec3(.25f);

		Generate();
		// testing
		wait = 0;

		Listeners();
	}
	
	void Update()
	{
		// testing
		//if(++wait > 60){ wait = 0; Toggle(rand() % CHUNK_X, rand() % CHUNK_Y, rand() % CHUNK_Z); }
		Entity->Get<StorageTransform>(id)->Rotation.z += 1.f;
			
		auto cnk = Entity->Get<StorageChunk>(id);
		auto frm = Entity->Get<StorageForm>(id);

		if(cnk->changed)
		{
			cnk->changed = false;

			vector<float> Position, Color;
			vector<int> Elements;
			int n = 0;
			for(int i = 0; i < CHUNK_X; ++i)
			for(int j = 0; j < CHUNK_Y; ++j)
			for(int k = 0; k < CHUNK_Z; ++k)
			{
				if(!Get(i, j, k)) continue;
				int x = i-(CHUNK_X/2), y = j-(CHUNK_Y/2), z = k-(CHUNK_Z/2);
				Position.push_back(x-.5f); Position.push_back(y-.5f); Position.push_back(z+.5f);
				Position.push_back(x+.5f); Position.push_back(y-.5f); Position.push_back(z+.5f);
				Position.push_back(x+.5f); Position.push_back(y+.5f); Position.push_back(z+.5f);
				Position.push_back(x-.5f); Position.push_back(y+.5f); Position.push_back(z+.5f);
				Position.push_back(x-.5f); Position.push_back(y-.5f); Position.push_back(z-.5f);
				Position.push_back(x+.5f); Position.push_back(y-.5f); Position.push_back(z-.5f);
				Position.push_back(x+.5f); Position.push_back(y+.5f); Position.push_back(z-.5f);
				Position.push_back(x-.5f); Position.push_back(y+.5f); Position.push_back(z-.5f);
				Color.push_back(0.f); Color.push_back(1.f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(0.f); Color.push_back(1.f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(0.f); Color.push_back(1.f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(0.f); Color.push_back(1.f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(.4f); Color.push_back(.2f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(.4f); Color.push_back(.2f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(.4f); Color.push_back(.2f); Color.push_back(0.f); Color.push_back(1.f);
				Color.push_back(.4f); Color.push_back(.2f); Color.push_back(0.f); Color.push_back(1.f);
				if(!Get(i + 1, j, k)) // left front
				{
					Elements.push_back(n+1); Elements.push_back(n+5); Elements.push_back(n+6);
					Elements.push_back(n+6); Elements.push_back(n+2); Elements.push_back(n+1);
				}
				if(!Get(i, j + 1, k)) // right front
				{
					Elements.push_back(n+3); Elements.push_back(n+2); Elements.push_back(n+6);
					Elements.push_back(n+6); Elements.push_back(n+7); Elements.push_back(n+3);
				}
				if(!Get(i, j, k + 1)) // top
				{
					Elements.push_back(n+0); Elements.push_back(n+1); Elements.push_back(n+2);
					Elements.push_back(n+2); Elements.push_back(n+3); Elements.push_back(n+0);
				}
				if(!Get(i - 1, j, k)) // right back
				{
					Elements.push_back(n+4); Elements.push_back(n+0); Elements.push_back(n+3);
					Elements.push_back(n+3); Elements.push_back(n+7); Elements.push_back(n+4);
				}
				if(!Get(i, j - 1, k)) // left back
				{
					Elements.push_back(n+4); Elements.push_back(n+5); Elements.push_back(n+1);
					Elements.push_back(n+1); Elements.push_back(n+0); Elements.push_back(n+4);
				}
				if(!Get(i, j, k - 1)) // bottom
				{
					Elements.push_back(n+7); Elements.push_back(n+6); Elements.push_back(n+5);
					Elements.push_back(n+5); Elements.push_back(n+4); Elements.push_back(n+7);
				}
				n += 8;
			}

			glGenBuffers(1, &frm->VerticesPosition);
			glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesPosition);
			glBufferData(GL_ARRAY_BUFFER, Position.size() * sizeof(float), &Position[0], GL_STATIC_DRAW);

			glGenBuffers(1, &frm->VerticesColor);
			glBindBuffer(GL_ARRAY_BUFFER, frm->VerticesColor);
			glBufferData(GL_ARRAY_BUFFER, Color.size() * sizeof(float), &Color[0], GL_STATIC_DRAW);

			glGenBuffers(1, &frm->Elements);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(int), &Elements[0], GL_STATIC_DRAW);
		}

	}

	void Listeners()
	{
		Event->Listen("SystemInitilized", [=]{
			auto frm = Entity->Get<StorageForm>(id);
			frm->Program = Global->Get<StorageShader>("shader")->Program;
		});
	}

	void Generate()
	{
		for(int x = 0; x < CHUNK_X; ++x)
		for(int y = 0; y < CHUNK_Y; ++y)
		{
			int height = (int)((simplex(vec2((float)x / CHUNK_X, (float)y / CHUNK_Y))/2+.5f) * CHUNK_Z);
			for(int z = 0; z < height; ++z)
			{
				Set(x, y, z, true);
			}
		}		
	}

	bool Get(int x, int y, int z) {
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return false;
		auto cnk = Entity->Get<StorageChunk>(id);
		return cnk->blocks[x][y][z];
	}

	void Set(int x, int y, int z, bool solid) {
		auto cnk = Entity->Get<StorageChunk>(id);
		cnk->blocks[x][y][z] = solid;
		cnk->changed = true;
	}

	void Toggle(int x, int y, int z)
	{
		Set(x, y, z, !Get(x, y, z));
	}

};