#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
#include <cstdlib>
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
#include "terrain.h"
#include "shader.h"
#include "movement.h"


#define TILES_U 4
#define TILES_V 4

typedef detail::tvec3<int> vec3i;

class ComponentTerrain : public Component
{
	void Init()
	{
		auto wld = Global->Add<StorageTerrain>("terrain");

		Listeners();
	}
	
	void Update()
	{
		auto cks = Entity->Get<StorageChunk>();

		for(auto chunk : cks)
			if(chunk.second->changed)
			{
				Mesh(chunk.first);
				chunk.second->changed = false;
			}
	}

	void Listeners()
	{
		Event->Listen("SystemInitialized", [=]{

			// generate test chunks
			//
			//     0 1 2 3 4 5 6
			//
			// 0   X X X X X X X
			// 1   X       X X X
			// 2   X       X X X
			// 3   X       X X X
			// 4   X X X X X X X
			// 5   X X X X X X X
			// 6   X X X X X X X


			for(int x = 0; x < 7; ++x)
			for(int z = 0; z < 7; ++z)
				addChunk(x, 0, z);
			
			for(int x = 1; x < 4; ++x)
			for(int z = 1; z < 4; ++z)
				deleteChunk(x, 0, z);
		});
	}

	unsigned int getChunk(int X, int Y, int Z)
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		array<int, 3> key = {X, Y, Z};
		auto i = wld->chunks.find(key);
		return (i != wld->chunks.end()) ? i->second : 0;
	}

	void addChunk(int X, int Y, int Z)
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		auto shd = Global->Get<StorageShader>("shader");

		unsigned int id = getChunk(X, Y, Z);
		if(id > 0) return;

		id = Entity->New();
		Entity->Add<StorageChunk>(id);
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);

		frm->Program = shd->Program;
		tsf->Position = vec3(X * CHUNK_X, Y * CHUNK_Y, Z * CHUNK_Z);

		Generate(id, X, Y, Z);

		array<int, 3> key = {X, Y, Z};
		wld->chunks.insert(make_pair(key, id));
	}

	void deleteChunk(int X, int Y, int Z)
	{
		auto wld = Global->Get<StorageTerrain>("terrain");

		unsigned int id = getChunk(X, Y, Z);
		if(id < 1) return;

		array<int, 3> key = {X, Y, Z};
		wld->chunks.erase(key);

		Entity->Delete<StorageChunk>(id);
		Entity->Delete<StorageForm>(id);
		Entity->Delete<StorageTransform>(id);
	}

	bool getBlock(vec3 pos)
	{
		return getBlock((int)pos.x, (int)pos.y, (int)pos.z);
	}

	bool getBlock(int x, int y, int z) {
		int X = x / CHUNK_X; x %= CHUNK_X;
		int Y = y / CHUNK_Y; y %= CHUNK_Y;
		int Z = z / CHUNK_Z; z %= CHUNK_Z;
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return false;

		auto cnk = Entity->Get<StorageChunk>(getChunk(X, Y, Z));
		return cnk->blocks[x][y][z];
	}

	void setBlock(int x, int y, int z, bool enabled) {
		int X = x / CHUNK_X; x %= CHUNK_X;
		int Y = y / CHUNK_Y; y %= CHUNK_Y;
		int Z = z / CHUNK_Z; z %= CHUNK_Z;
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return;

		auto cnk = Entity->Get<StorageChunk>(getChunk(X, Y, Z));
		cnk->blocks[x][y][z] = enabled;
		cnk->changed = true;
	}

	void Generate(unsigned int id, int X, int Y, int Z)
	{
		auto cnk = Entity->Get<StorageChunk>(id);
		cnk->changed = true;

		for(int x = 0; x < CHUNK_X; ++x)
		for(int z = 0; z < CHUNK_Z; ++z)
		{
			double height_base = simplex(vec2((float)(x+X*CHUNK_X) / CHUNK_X, (float)z / (z+Z*CHUNK_Z))) / 3 + .3f;
			double height_fine = simplex(vec2(5 * (float)(x+X*CHUNK_X) / CHUNK_X, 5 * (float)z / (z+Z*CHUNK_Z))) * 0.05;
			int height = (int)((height_base + height_fine) * CHUNK_Y);
			for(int y = 0; y < height; ++y) cnk->blocks[x][y][z] = true;
			cnk->blocks[x][0][z] = true;
		}
	}

	void Mesh(unsigned int id)
	{
		auto cnk = Entity->Get<StorageChunk>(id);
		auto frm = Entity->Get<StorageForm>(id);

		// vertices

		vector<float> Vertices, Normals, Texcoords;
		vector<int> Elements;

		const vec2 grid(1.f / TILES_U, 1.f / TILES_V);

		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X)
		for(int Y = 0; Y < CHUNK_Y; ++Y)
		for(int Z = 0; Z < CHUNK_Z; ++Z)
			if(cnk->blocks[X][Y][Z])
			{
				int Tile = Clamp(rand() % 2 + 1, 0, TILES_U * TILES_V - 1);

				for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
					vec3i neigh = Shift(dim, vec3i(dir, 0, 0)) + vec3i(X, Y, Z);
					if(!cnk->blocks[neigh.x][neigh.y][neigh.z] || !Inside(neigh, vec3i(0), vec3i(CHUNK_X, CHUNK_Y, CHUNK_Z) - 1))
					{
						for(float i = 0; i <= 1; ++i)
						for(float j = 0; j <= 1; ++j)
						{
							vec3 vertex = vec3(X, Y, Z) + floatify(Shift(dim, vec3i((dir+1)/2, i, j)));
							Vertices.push_back(vertex.x); Vertices.push_back(vertex.y); Vertices.push_back(vertex.z);
						}

						vec3 normal = normalize(floatify(Shift(dim, vec3i(dir, 0, 0))));
						for(int i = 0; i < 4; ++i)
						{
							Normals.push_back(normal.x); Normals.push_back(normal.y); Normals.push_back(normal.z);
						}

						vec2 position = (vec2(Tile % TILES_U, Tile / TILES_U) + .25f) * grid;
						Texcoords.push_back(position.x);            Texcoords.push_back(position.y);
						Texcoords.push_back(position.x + grid.x/2); Texcoords.push_back(position.y);
						Texcoords.push_back(position.x);            Texcoords.push_back(position.y + grid.y/2);
						Texcoords.push_back(position.x + grid.x/2); Texcoords.push_back(position.y + grid.y/2);

						if(dir == -1) {
							Elements.push_back(n+0); Elements.push_back(n+1); Elements.push_back(n+2);
							Elements.push_back(n+1); Elements.push_back(n+3); Elements.push_back(n+2);
						} else {
							Elements.push_back(n+0); Elements.push_back(n+2); Elements.push_back(n+1);
							Elements.push_back(n+1); Elements.push_back(n+2); Elements.push_back(n+3);
						}
						n += 4;
					}
				dir *= -1; } while(dir > 0); }
			}

		// buffers

		glGenBuffers(1, &frm->Positions);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), &Vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(float), &Normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, Texcoords.size() * sizeof(float), &Texcoords[0], GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(int), &Elements[0], GL_STATIC_DRAW);

		// texture
		
		glGenTextures(1, &frm->Texture);
		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Image image;
		bool result = image.loadFromFile("forms/textures/terrain.png");
		if(!result){ Debug::Fail("Terrain texture loading fail"); return; }

		Vector2u size = Vector2u(image.getSize().x / TILES_U, image.getSize().y / TILES_V);
		Image texture;
		texture.create(image.getSize().x * 2, image.getSize().y * 2, Color());
		for(int u = 0; u < TILES_U; ++u)
		for(int v = 0; v < TILES_V; ++v)
		{
			Image tile, quarter;
			tile.create(size.x, size.y, Color());
			tile.copy(image, 0, 0, IntRect(size.x * u, size.y * v, size.x, size.y), true);
			quarter.create(size.x, size.y, Color());
			quarter.copy(tile, 0,          0,          IntRect(size.x / 2, size.y / 2, size.x / 2, size.y / 2), true);
			quarter.copy(tile, size.x / 2, 0,          IntRect(0,          size.y / 2, size.x / 2, size.y / 2), true);
			quarter.copy(tile, 0,          size.y / 2, IntRect(size.x / 2, 0,          size.x / 2, size.y / 2), true);
			quarter.copy(tile, size.x / 2, size.y / 2, IntRect(0,          0,          size.x / 2, size.y / 2), true);
			texture.copy(quarter, (u * 2    ) * size.x, (v * 2    ) * size.y, IntRect(0, 0, 0, 0), true);
			texture.copy(quarter, (u * 2 + 1) * size.x, (v * 2    ) * size.y, IntRect(0, 0, 0, 0), true);
			texture.copy(quarter, (u * 2    ) * size.x, (v * 2 + 1) * size.y, IntRect(0, 0, 0, 0), true);
			texture.copy(quarter, (u * 2 + 1) * size.x, (v * 2 + 1) * size.y, IntRect(0, 0, 0, 0), true);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize().x, texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	template <typename T>
	inline T Clamp(T Value, T Min, T Max)
	{
		if(Value < Min) return Min;
		if(Value > Max) return Max;
		return Value;
	}

	bool Inside(vec3i Position, vec3i Min, vec3i Max)
	{
		if(Position.x < Min.x || Position.y < Min.y || Position.z < Min.z) return false;
		if(Position.x > Max.x || Position.y > Max.y || Position.z > Max.z) return false;
		return true;
	}

	inline vec3i Shift(int Dimension, vec3i Vector)
	{
		if      (Dimension % 3 == 1) return vec3i(Vector.z, Vector.x, Vector.y);
		else if (Dimension % 3 == 2) return vec3i(Vector.y, Vector.z, Vector.x);
		else                         return Vector;
	}

	vec3 floatify(vec3i Value)
	{
		return vec3(Value.x, Value.y, Value.z);
	}
};