#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;

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
		Entity->Add<StorageTransform>(id);
		Entity->Add<StorageForm>(id);

		Generate();

		Listeners();
	}
	
	void Update()
	{
		auto cnk = Entity->Get<StorageChunk>(id);
		auto frm = Entity->Get<StorageForm>(id);

		if(!cnk->changed) return;
		cnk->changed = false;

		Debug::Pass("Terrain calculation vertices");

		vector<float> Position, Color;
		vector<int> Elements;
		
		CreateB(&Position, &Color, &Elements);

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
		for(int z = 0; z < CHUNK_Z; ++z)
		{
			int height = (int)((simplex(vec2((float)x / CHUNK_X, (float)z / CHUNK_Z))/2+.5f) * CHUNK_Y);
			for(int y = 0; y < height; ++y)
			{
				Set(x, y, z, true);
			}
		}
	}

	bool Get(vec3 pos)
	{
		return Get((int)pos.x, (int)pos.y, (int)pos.z);
	}

	bool Get(int x, int y, int z) {
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return false;
		auto cnk = Entity->Get<StorageChunk>(id);
		return cnk->blocks[x][y][z];
	}

	void Set(int x, int y, int z, bool solid) {
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return;
		auto cnk = Entity->Get<StorageChunk>(id);
		cnk->blocks[x][y][z] = solid;
		cnk->changed = true;
	}

	inline vec3 Shift(int Dimension, vec3 Vector)
	{
		if      (Dimension % 3 == 1) return vec3(Vector.z, Vector.x, Vector.y);
		else if (Dimension % 3 == 2) return vec3(Vector.y, Vector.z, Vector.x);
		else                         return Vector;
	}

	void CreateA(vector<float> *Position, vector<float> *Color, vector<int> *Elements)
	// 1520 triangles per second and 3.1 vertices per triangle
	// generated chunk has 0, 0, 0 at its center
	{
		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X)
		for(int Y = 0; Y < CHUNK_Y; ++Y)
		for(int Z = 0; Z < CHUNK_Z; ++Z)
			if(Get(X, Y, Z))
			{
				bool next_pos_i = Get(X+1, Y, Z), next_neg_i = Get(X-1, Y, Z), next_pos_j = Get(X, Y+1, Z), next_neg_j = Get(X, Y-1, Z), next_pos_k = Get(X, Y, Z+1), next_neg_k = Get(X, Y, Z-1);
				if(next_pos_i && next_neg_i && next_pos_j && next_neg_j && next_pos_k && next_neg_k) continue;

				// vertices
				Position->push_back(X-.5f); Position->push_back(Y-.5f); Position->push_back(Z+.5f);
				Position->push_back(X+.5f); Position->push_back(Y-.5f); Position->push_back(Z+.5f);
				Position->push_back(X+.5f); Position->push_back(Y+.5f); Position->push_back(Z+.5f);
				Position->push_back(X-.5f); Position->push_back(Y+.5f); Position->push_back(Z+.5f);
				Position->push_back(X-.5f); Position->push_back(Y-.5f); Position->push_back(Z-.5f);
				Position->push_back(X+.5f); Position->push_back(Y-.5f); Position->push_back(Z-.5f);
				Position->push_back(X+.5f); Position->push_back(Y+.5f); Position->push_back(Z-.5f);
				Position->push_back(X-.5f); Position->push_back(Y+.5f); Position->push_back(Z-.5f);

				// colors
				Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);
				Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);

				// elements
				if(!next_pos_i) { Elements->push_back(n+1); Elements->push_back(n+5); Elements->push_back(n+6); Elements->push_back(n+6); Elements->push_back(n+2); Elements->push_back(n+1); }
				if(!next_neg_i) { Elements->push_back(n+4); Elements->push_back(n+0); Elements->push_back(n+3); Elements->push_back(n+3); Elements->push_back(n+7); Elements->push_back(n+4); }
				if(!next_pos_j) { Elements->push_back(n+3); Elements->push_back(n+2); Elements->push_back(n+6); Elements->push_back(n+6); Elements->push_back(n+7); Elements->push_back(n+3); }
				if(!next_neg_j) { Elements->push_back(n+4); Elements->push_back(n+5); Elements->push_back(n+1); Elements->push_back(n+1); Elements->push_back(n+0); Elements->push_back(n+4); }
				if(!next_pos_k) { Elements->push_back(n+0); Elements->push_back(n+1); Elements->push_back(n+2); Elements->push_back(n+2); Elements->push_back(n+3); Elements->push_back(n+0); }
				if(!next_neg_k) { Elements->push_back(n+7); Elements->push_back(n+6); Elements->push_back(n+5); Elements->push_back(n+5); Elements->push_back(n+4); Elements->push_back(n+7); }
				n += 8;
			}
	}

	void CreateB(vector<float> *Position, vector<float> *Color, vector<int> *Elements)
	// 1543 triangles per second and somehow 2.0 vertices per triangle
	// generated chunk has 0, 0, 0 at its front bottom right corner point 
	{
		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X)
		for(int Y = 0; Y < CHUNK_Y; ++Y)
		for(int Z = 0; Z < CHUNK_Z; ++Z)
			if(Get(X, Y, Z))
				for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
					if(!Get(vec3(X, Y, Z) + Shift(dim, vec3(dir, 0, 0))))
					{
						// faces
						for(float i = 0; i <= 1; ++i)
						for(float j = 0; j <= 1; ++j)
						{
							// vertices
							vec3 vertex = vec3(X, Y, Z) + Shift(dim, vec3((dir+1)/2, i, j));
							Position->push_back(vertex.x); Position->push_back(vertex.y); Position->push_back(vertex.z);
						}

						// colors
						Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);
						Color->push_back(0.f); Color->push_back(1.f); Color->push_back(0.f); Color->push_back(1.f);
						Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);
						Color->push_back(.4f); Color->push_back(.2f); Color->push_back(0.f); Color->push_back(1.f);

						// elements
						if(dir == -1) {
							Elements->push_back(n+0); Elements->push_back(n+1); Elements->push_back(n+2);
							Elements->push_back(n+1); Elements->push_back(n+3); Elements->push_back(n+2);
						} else {
							Elements->push_back(n+0); Elements->push_back(n+2); Elements->push_back(n+1);
							Elements->push_back(n+1); Elements->push_back(n+2); Elements->push_back(n+3);
						}
						n += 4;
					}
				dir *= -1; } while(dir > 0); }
	}
};