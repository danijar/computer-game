#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
using namespace sf;

#include "form.h"
#include "transform.h"
#include "chunk.h"


class ComponentTerrain : public Component
{
	unsigned int id;
	Clock clock;

	void Init()
	{
		/*
		id = Entity->New();
		Entity->Add<StorageChunk>(id);
		Entity->Add<StorageTransform>(id);
		Entity->Add<StorageForm>(id);

		Set(0,  0,  0, true);
		Set(0,  0,  2, true);
		Set(0,  2,  0, true);

		Set(5,  5,  5, true);
		Set(5,  5,  7, true);
		Set(5,  7,  5, true);

		Set(10,  10,  10, true);
		Set(10,  10,  12, true);
		Set(10,  12,  10, true);
		*/
	}

	void Update()
	{
		float time = clock.getElapsedTime().asSeconds();
		/*
		auto cnk = Entity->Get<StorageChunk>(id);
		if(cnk->changed)
		{
			cnk->changed = false;

			vector<float> Vertices;
			vector<int> Elements;
			int i = 0;
			for(int x = 0; x < CHUNK_X; ++x)
			{
				for(int y = 0; y < CHUNK_Y; ++y)
				{
					for(int z = 0; z < CHUNK_Z; ++z)
					{
						if(!Get(x, y, z)) continue;
						// vertices
						//                   X                          Y                          Z                         R                         G                         B                         A
						Vertices.push_back(x-.5f); Vertices.push_back(y-.5f); Vertices.push_back(z+.5f); Vertices.push_back(+.5f); Vertices.push_back(-.5f); Vertices.push_back(-.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x+.5f); Vertices.push_back(y-.5f); Vertices.push_back(z+.5f); Vertices.push_back(-.5f); Vertices.push_back(+.5f); Vertices.push_back(-.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x+.5f); Vertices.push_back(y+.5f); Vertices.push_back(z+.5f); Vertices.push_back(-.5f); Vertices.push_back(-.5f); Vertices.push_back(+.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x-.5f); Vertices.push_back(y+.5f); Vertices.push_back(z+.5f); Vertices.push_back(+.5f); Vertices.push_back(+.5f); Vertices.push_back(+.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x-.5f); Vertices.push_back(y-.5f); Vertices.push_back(z-.5f); Vertices.push_back(-.5f); Vertices.push_back(-.5f); Vertices.push_back(+.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x+.5f); Vertices.push_back(y-.5f); Vertices.push_back(z-.5f); Vertices.push_back(+.5f); Vertices.push_back(+.5f); Vertices.push_back(+.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x+.5f); Vertices.push_back(y+.5f); Vertices.push_back(z-.5f); Vertices.push_back(+.5f); Vertices.push_back(-.5f); Vertices.push_back(-.5f); Vertices.push_back(.8f); 
						Vertices.push_back(x-.5f); Vertices.push_back(y+.5f); Vertices.push_back(z-.5f); Vertices.push_back(-.5f); Vertices.push_back(+.5f); Vertices.push_back(-.5f); Vertices.push_back(.8f);
						// elements
						Elements.push_back(0); Elements.push_back(1); Elements.push_back(2); Elements.push_back(2); Elements.push_back(3); Elements.push_back(0); 
						Elements.push_back(1); Elements.push_back(5); Elements.push_back(6); Elements.push_back(6); Elements.push_back(2); Elements.push_back(1); 
						Elements.push_back(7); Elements.push_back(6); Elements.push_back(5); Elements.push_back(5); Elements.push_back(4); Elements.push_back(7); 
						Elements.push_back(4); Elements.push_back(0); Elements.push_back(3); Elements.push_back(3); Elements.push_back(7); Elements.push_back(4); 
						Elements.push_back(4); Elements.push_back(5); Elements.push_back(1); Elements.push_back(1); Elements.push_back(0); Elements.push_back(4); 
						Elements.push_back(3); Elements.push_back(2); Elements.push_back(6); Elements.push_back(6); Elements.push_back(7); Elements.push_back(3);
						i += 8;
					}
				}
			}
			// generate buffer
			auto frm = Entity->Get<StorageForm>(id);
			frm->Quantity = Elements.size();

			glGenBuffers(1, &frm->VertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, frm->VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, &frm->ElementBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->ElementBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size(), &Elements[0], GL_STATIC_DRAW);
		}
			*/
	}
	/*
	uint8_t Get(int x, int y, int z) {
		auto cnk = Entity->Get<StorageChunk>(id);
		return cnk->blocks[x][y][z];
	}

	void Set(int x, int y, int z, bool solid) {
		auto cnk = Entity->Get<StorageChunk>(id);
		cnk->blocks[x][y][z] = solid;
		cnk->changed = true;
	}
	*/
};