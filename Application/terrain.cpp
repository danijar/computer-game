#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
using namespace sf;

#include "form.h"
#include "transform.h"
#include "chunk.h"
#include "shader.h"
#include "movement.h"


class ComponentTerrain : public Component
{
	unsigned int id;

	void Init()
	{
		id = Entity->New();
		Entity->Add<StorageChunk>(id);
		auto tsf = Entity->Add<StorageTransform>(id);
		auto frm = Entity->Add<StorageForm>(id);
		Entity->Add<StorageMovement>(id);

		tsf->Position = vec3(-5);
		tsf->Rotation = vec3(0, 0, 1);
		tsf->Angle = 0;
		frm->Scale = vec3(.25f);

		for(int x = 0; x < CHUNK_X; ++x) for(int y = 0; y < CHUNK_Y; ++y) Set(x, y, 0, true);
		Set(4, 4,  1, true);
		Set(4, 5,  1, true);
		Set(5, 4,  1, true);
		Set(5, 5,  1, true);
		Set(5, 6,  1, true);
		Set(4, 6,  1, true);
		Set(6, 5,  1, true);
		Set(6, 4,  1, true);
		Set( 0,  0,  0, true);
		Set(15,  0,  0, true);
		Set( 0, 15,  0, true);
		Set(15, 15,  0, true);
		Set( 0,  0, 15, true);
		Set(15,  0, 15, true);
		Set( 0, 15, 15, true);
		Set(15, 15, 15, true);

		Listeners();
	}

	void Update()
	{
		auto cnk = Entity->Get<StorageChunk>(id);
		auto frm = Entity->Get<StorageForm>(id);

		if(cnk->changed)
		{
			cnk->changed = false;

			vector<float> Position, Color;
			vector<int> Elements;
			int i = 0;
			for(int ix = 0; ix < CHUNK_X; ++ix)
			for(int iy = 0; iy < CHUNK_Y; ++iy)
			for(int iz = 0; iz < CHUNK_Z; ++iz)
			{
				if(!Get(ix, iy, iz)) continue;
				int x = ix-(CHUNK_X/2), y = iy-(CHUNK_Y/2), z = iz-(CHUNK_Z/2);
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
				if(!Get(ix + 1, iy, iz)) // left front
				{
					Elements.push_back(i+1); Elements.push_back(i+5); Elements.push_back(i+6);
					Elements.push_back(i+6); Elements.push_back(i+2); Elements.push_back(i+1);
				}
				if(!Get(ix, iy + 1, iz)) // right front
				{
					Elements.push_back(i+3); Elements.push_back(i+2); Elements.push_back(i+6);
					Elements.push_back(i+6); Elements.push_back(i+7); Elements.push_back(i+3);
				}
				if(!Get(ix, iy, iz + 1)) // top
				{
					Elements.push_back(i+0); Elements.push_back(i+1); Elements.push_back(i+2);
					Elements.push_back(i+2); Elements.push_back(i+3); Elements.push_back(i+0);
				}
				if(!Get(ix - 1, iy, iz)) // right back
				{
					Elements.push_back(i+4); Elements.push_back(i+0); Elements.push_back(i+3);
					Elements.push_back(i+3); Elements.push_back(i+7); Elements.push_back(i+4);
				}
				if(!Get(ix, iy - 1, iz)) // left back
				{
					Elements.push_back(i+4); Elements.push_back(i+5); Elements.push_back(i+1);
					Elements.push_back(i+1); Elements.push_back(i+0); Elements.push_back(i+4);
				}
				if(!Get(ix, iy, iz - 1)) // bottom
				{
					Elements.push_back(i+7); Elements.push_back(i+6); Elements.push_back(i+5);
					Elements.push_back(i+5); Elements.push_back(i+4); Elements.push_back(i+7);
				}
				i += 8;
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

};