#pragma once

#include "system.h"
#include "debug.h"

#include <vector>
#include <cstdlib>
#include <future>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;

#include "settings.h"
#include "camera.h"
#include "form.h"
#include "transform.h"
#include "terrain.h"
#include "shader.h"
#include "movement.h"


class ComponentTerrain : public Component
{
	void Init()
	{
		auto wld = Global->Add<StorageTerrain>("terrain");

		tasking = false;

		Texture();

		Listeners();
	}

	void Update()
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		auto stg = Global->Get<StorageSettings>("settings");
		auto cam = Global->Get<StorageCamera>("camera");
		auto cks = Entity->Get<StorageChunk>();

		int Distance = (int)(.5f * stg->Viewdistance / CHUNK_X / 2);
		ivec3 Camera = intify(cam->Position / vec3(CHUNK_X, CHUNK_Y, CHUNK_Z));
		for(int X = -Distance; X <= Distance; ++X)
		for(int Z = -Distance; Z <= Distance; ++Z)
		if(X * X + Z * Z <= Distance * Distance)
		{
			addChunk(ivec3(X + Camera.x, 0, Z + Camera.z));
		}
		for(auto chunk : wld->chunks)
		{
			if((signed)(chunk.first - Camera).length() > Distance)
				deleteChunk(chunk.first);
		}

		if(tasking)
		{
			if(task.wait_for(chrono::milliseconds(0)) == future_status::ready)
			{
				tasking = false;
				Data data = task.get();
				Buffers(data);
			}
		}
		else
		{
			for(auto chunk : cks)
			if(chunk.second->changed)
			{
				tasking = true;
				chunk.second->changed = false;
				task = async(launch::async, &ComponentTerrain::Mesh, this, Data(chunk.first));
				break;
			}
		}
	}

	struct Data
	{
		Data() {}
		Data(unsigned int id) : id(id) {}
		unsigned int id;
		vector<float> Vertices, Normals, Texcoords;
		vector<int> Elements;
	};

	future<Data> task;
	bool tasking;
	Image texture;

	void Listeners()
	{
		Event->Listen("SystemInitialized", [=]{
			auto cam = Global->Get<StorageCamera>("camera");
			cam->Position = vec3(0, CHUNK_Y, 0);
			cam->Angles = vec2(0.75, -0.25);
		});
	}

	unsigned int getChunk(ivec3 key)
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		auto i = wld->chunks.find(key);
		return (i != wld->chunks.end()) ? i->second : 0;
	}

	int addChunk(ivec3 key)
	{
		unsigned int id = getChunk(key);
		if(!id)
		{
			id = Entity->New();
			Entity->Add<StorageChunk>(id);
			auto tsf = Entity->Add<StorageTransform>(id);

			tsf->Position = key * CHUNK;

			Generate(id, key);

			Global->Get<StorageTerrain>("terrain")->chunks.insert(make_pair(key, id));
		}
		return id;
	}

	void deleteChunk(ivec3 key)
	{
		unsigned int id = getChunk(key);
		if(id < 1) return;

		Debug::Info("Terrain delete chunk " + to_string(key.x) + " " + to_string(key.y) + " " + to_string(key.z));

		Global->Get<StorageTerrain>("terrain")->chunks.erase(key);

		auto frm = Entity->Add<StorageForm>(id);
		glDeleteBuffers(1, &frm->Positions);
		glDeleteBuffers(1, &frm->Normals);
		glDeleteBuffers(1, &frm->Texcoords);
		glDeleteBuffers(1, &frm->Elements);
		glDeleteTextures(1, &frm->Texture);

		Entity->Delete<StorageChunk>(id);
		Entity->Delete<StorageForm>(id);
		Entity->Delete<StorageTransform>(id);
	}

	/*
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
		setBlock(X, Y, Z, x, y, z, enabled);
	}

	void setBlock(int X, int Y, int Z, int x, int y, int z, bool enabled) {
		if(x < 0 || y < 0 || z < 0 || x > CHUNK_X-1 || y > CHUNK_Y-1 || z > CHUNK_Z-1) return;

		auto cnk = Entity->Get<StorageChunk>(getChunk(X, Y, Z));
		cnk->blocks[x][y][z] = enabled;
		cnk->changed = true;
	}
	*/

	void Generate(unsigned int id, ivec3 key)
	{
		auto cnk = Entity->Get<StorageChunk>(id);
		cnk->changed = true;

		for(int x = 0; x < CHUNK_X; ++x) {
		const float i = key.x + (float)x / CHUNK_X;
		for(int z = 0; z < CHUNK_Z; ++z) {
		const float j = key.z + (float)z / CHUNK_Z;
				double height_bias = 0.30;
				double height_base = 0.50 * (simplex(0.2f * vec2(i, j)) + 1) / 2;
				double height_fine = 0.20 * (simplex(1.5f * vec2(i, j)) + 1) / 2;
				int height = (int)((height_bias + height_base + height_fine) * CHUNK_Y);
				for(int y = 0; y < height; ++y) cnk->blocks[x][y][z] = true;
		} }
	}

	#define TILES_U 4
	#define TILES_V 4

	Data Mesh(Data data)
	{
		auto cnk = Entity->Get<StorageChunk>(data.id);

		auto *Vertices = &data.Vertices, *Normals = &data.Normals, *Texcoords = &data.Texcoords;
		auto *Elements = &data.Elements;

		const vec2 grid(1.f / TILES_U, 1.f / TILES_V);

		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X)
		for(int Y = 0; Y < CHUNK_Y; ++Y)
		for(int Z = 0; Z < CHUNK_Z; ++Z)
			if(cnk->blocks[X][Y][Z])
			{
				int Tile = Clamp(rand() % 2 + 1, 0, TILES_U * TILES_V - 1);

				for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
					ivec3 neigh = Shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

					if(Inside(neigh, ivec3(0), CHUNK - 1))
						if(cnk->blocks[neigh.x][neigh.y][neigh.z])
							{ dir *= -1; continue; }

					for(float i = 0; i <= 1; ++i)
					for(float j = 0; j <= 1; ++j)
					{
						vec3 vertex = vec3(X, Y, Z) + floatify(Shift(dim, ivec3((dir+1)/2, i, j)));
						Vertices->push_back(vertex.x); Vertices->push_back(vertex.y); Vertices->push_back(vertex.z);
					}

					vec3 normal = normalize(floatify(Shift(dim, ivec3(dir, 0, 0))));
					for(int i = 0; i < 4; ++i)
					{
						Normals->push_back(normal.x); Normals->push_back(normal.y); Normals->push_back(normal.z);
					}

					vec2 position = (vec2(Tile % TILES_U, Tile / TILES_U) + .25f) * grid;
					Texcoords->push_back(position.x);            Texcoords->push_back(position.y);
					Texcoords->push_back(position.x + grid.x/2); Texcoords->push_back(position.y);
					Texcoords->push_back(position.x);            Texcoords->push_back(position.y + grid.y/2);
					Texcoords->push_back(position.x + grid.x/2); Texcoords->push_back(position.y + grid.y/2);

					if(dir == -1) {
						Elements->push_back(n+0); Elements->push_back(n+1); Elements->push_back(n+2);
						Elements->push_back(n+1); Elements->push_back(n+3); Elements->push_back(n+2);
					} else {
						Elements->push_back(n+0); Elements->push_back(n+2); Elements->push_back(n+1);
						Elements->push_back(n+1); Elements->push_back(n+2); Elements->push_back(n+3);
					}
					n += 4;

				dir *= -1; } while(dir > 0); }
			}

		return data;
	}

	void Buffers(Data data)
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto frm = Entity->Add<StorageForm>(data.id);

		frm->Program = shd->Program;

		glGenBuffers(1, &frm->Positions);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Positions);
		glBufferData(GL_ARRAY_BUFFER, data.Vertices.size() * sizeof(float), &(data.Vertices[0]), GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, data.Normals.size() * sizeof(float), &(data.Normals[0]), GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, data.Texcoords.size() * sizeof(float), &(data.Texcoords[0]), GL_STATIC_DRAW);

		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.Elements.size() * sizeof(int), &data.Elements[0], GL_STATIC_DRAW);

		glGenTextures(1, &frm->Texture);
		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize().x, texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture()
	{
		Image image;
		bool result = image.loadFromFile("forms/textures/terrain.png");
		if(!result){ Debug::Fail("Terrain texture loading fail"); return; }

		Vector2u size = Vector2u(image.getSize().x / TILES_U, image.getSize().y / TILES_V);
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
	}

	template <typename T>
	inline T Clamp(T Value, T Min, T Max)
	{
		if(Value < Min) return Min;
		if(Value > Max) return Max;
		return Value;
	}

	bool Inside(ivec3 Position, ivec3 Min, ivec3 Max)
	{
		if(Position.x < Min.x || Position.y < Min.y || Position.z < Min.z) return false;
		if(Position.x > Max.x || Position.y > Max.y || Position.z > Max.z) return false;
		return true;
	}

	inline ivec3 Shift(int Dimension, ivec3 Vector)
	{
		if      (Dimension % 3 == 1) return ivec3(Vector.z, Vector.x, Vector.y);
		else if (Dimension % 3 == 2) return ivec3(Vector.y, Vector.z, Vector.x);
		else                         return Vector;
	}

	vec3 floatify(ivec3 Value)
	{
		return vec3(Value.x, Value.y, Value.z);
	}

	ivec3 intify(vec3 Value)
	{
		return ivec3(Value.x, Value.y, Value.z);
	}
};