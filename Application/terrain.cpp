#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
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

		vector<float> Positions, Normals, Texcoords;
		vector<int> Elements;
		
		Vertices(&Positions, &Normals, &Texcoords, &Elements);

		glGenBuffers(1, &frm->Positions);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(float), &Positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(float), &Normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, Texcoords.size() * sizeof(float), &Texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(int), &Elements[0], GL_STATIC_DRAW);

		Image image;
		bool result = image.loadFromFile("forms/textures/dirt.jpg");
		auto size = image.getSize();
		glGenTextures(1, &frm->Texture);
		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, result ? size.x : 1, result ? size.y : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, result ? image.getPixelsPtr() : nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

	void Vertices(vector<float> *Position, vector<float> *Normals, vector<float> *Texcoords, vector<int> *Elements)
	{
		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X)
		for(int Y = 0; Y < CHUNK_Y; ++Y)
		for(int Z = 0; Z < CHUNK_Z; ++Z)
			if(Get(X, Y, Z))
				for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
					if(!Get(vec3(X, Y, Z) + Shift(dim, vec3(dir, 0, 0))))
					{
						for(float i = 0; i <= 1; ++i)
						for(float j = 0; j <= 1; ++j)
						{
							vec3 vertex = vec3(X, Y, Z) + Shift(dim, vec3((dir+1)/2, i, j));
							Position->push_back(vertex.x); Position->push_back(vertex.y); Position->push_back(vertex.z);
						}

						vec3 normal = normalize(Shift(dim, vec3(dir, 0, 0)));
						for(int i = 0; i < 4; ++i)
						{
							Normals->push_back(normal.x); Normals->push_back(normal.y); Normals->push_back(normal.z);
						}

						Texcoords->push_back(0.f); Texcoords->push_back(0.f);
						Texcoords->push_back(1.f); Texcoords->push_back(0.f);
						Texcoords->push_back(0.f); Texcoords->push_back(1.f);
						Texcoords->push_back(1.f); Texcoords->push_back(1.f);

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