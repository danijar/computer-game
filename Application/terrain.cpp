#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"

#include <vector>
#include <cstdlib>
#include <future>
#include <atomic>
using namespace std;
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
#include "movement.h"
#include "text.h"


class ComponentTerrain : public Component
{
	GLuint texture;

	void Init()
	{
		auto wld = Global->Add<StorageTerrain>("terrain");

		active = 0;
		meshing = false;

		/*
		Entity->Add<StorageText>(Entity->New())->Text = [=]{
			auto cks = Entity->Get<StorageChunk>();
			string output = "Chunks " + to_string(cks.size()) + "\n\n";
			for(int z = -10; z <= 10; ++z)
			{
				output += "\n";
				for(int x = -10; x <= 10; ++x)
				{
					output += (wld->chunks.find(ivec3(x, 0, z)) != wld->chunks.end()) ? "O" : "·";
					output += " ";
				}
			}
			return output;
		};
		*/

		Listeners();
	}

	void Update()
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		auto stg = Global->Get<StorageSettings>("settings");
		auto cam = Global->Get<StorageCamera>("camera");

		const int distance = (unsigned int)((stg->Viewdistance / ((CHUNK.x + CHUNK.y + CHUNK.z) / 3.f)) / 5);
		const ivec2 camera   = ivec2((int)(cam->Position.x   / CHUNK.x), (int)(cam->Position.z   / CHUNK.z));

		vector<ivec3> keys;
		for(auto i = wld->chunks.begin(); i != wld->chunks.end(); ++i)
		{
			const int margin = 1;

			vec2 offset = vec2(camera.x, camera.y) - vec2(i->first.x, i->first.z);
			int away = (int)sqrt(offset.x * offset.x + offset.y * offset.y);
			if(away > distance + margin)
			{
				keys.push_back(i->first);
			}
		}
		for(auto i : keys) disableChunk(i);

		ivec2 i;
		for(i.x = -distance; i.x < distance; ++i.x) {
		for(i.y = -distance; i.y < distance; ++i.y) {
			if(i.x * i.x + i.y * i.y < distance * distance)
			{
				enableChunk(ivec3(i.x + camera.x, 0, i.y + camera.y));
			}
		}}

		if(!meshing)
		{
			if(active)
			{
				task.get();
				Buffers(active);
				Clear();
				active = 0;
			}
			auto cks = Entity->Get<StorageChunk>();
			for(auto i : cks) // find nearest
			if(i.second->changed)
			{
				i.second->changed = false;
				active = i.first;
				meshing = true;
				task = async(launch::async, &ComponentTerrain::Meshing, this);
				break;
			}
		}
	}

	void Listeners()
	{
		Event->Listen("SystemInitialized", [=]{
			texture = Texture();

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

	int enableChunk(ivec3 key)
	{
		unsigned int id = getChunk(key);
		if(!id)
		{
			id = Entity->New();
			Entity->Add<StorageChunk>(id);
			auto tsf = Entity->Add<StorageTransform>(id);
			auto wld = Global->Get<StorageTerrain>("terrain");

			tsf->Position = key * CHUNK;

			Generate(id, key);

			wld->chunks.insert(make_pair(key, id));
		}
		return id;
	}

	void disableChunk(ivec3 key)
	{
		unsigned int id = getChunk(key);
		if(id)
		{
			if(active == id)
			{
				Debug::Info("Killed meshing thread to disable chunk " + to_string(id) + "!");

				meshing = false;
				task.get();
				active = 0;
			}
			else
			{
				auto frm = Entity->Get<StorageForm>(id);

				glDeleteBuffers(1, &frm->Vertices);
				glDeleteBuffers(1, &frm->Normals);
				glDeleteBuffers(1, &frm->Texcoords);
				glDeleteBuffers(1, &frm->Elements);

				Entity->Delete<StorageForm>(id);
			}

			auto wld = Global->Get<StorageTerrain>("terrain");
			
			Entity->Delete<StorageChunk>(id);
			Entity->Delete<StorageTransform>(id);

			wld->chunks.erase(key);
		}
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
	#define GRID vec2(1.f / TILES_U, 1.f / TILES_V)
	#define GAP float(0.0001)

	future<void> task;
	atomic_bool meshing;
	int active;
	vector<float> Vertices, Normals, Texcoords; vector<int> Elements;

	void Meshing()
	{
		auto cnk = Entity->Get<StorageChunk>(active);

		Clear();
		int n = 0;
		for(int X = 0; X < CHUNK_X; ++X) {
		for(int Y = 0; Y < CHUNK_Y; ++Y) {
		for(int Z = 0; Z < CHUNK_Z; ++Z) {
			if(this->meshing)
			{
				if(cnk->blocks[X][Y][Z])
				{
					int Tile = clamp(rand() % 2 + 1, 0, TILES_U * TILES_V - 1);
					for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
						ivec3 neigh = Shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

						if(Inside(neigh, ivec3(0), CHUNK - 1))
							if(cnk->blocks[neigh.x][neigh.y][neigh.z])
								goto skip;

						{
							for(float i = 0; i <= 1; ++i)
							for(float j = 0; j <= 1; ++j)
							{
								vec3 vertex = vec3(X, Y, Z) + vec3(Shift(dim, ivec3((dir+1)/2, i, j)));
								Vertices.push_back(vertex.x); Vertices.push_back(vertex.y); Vertices.push_back(vertex.z);
							}

							vec3 normal = normalize(vec3(Shift(dim, ivec3(dir, 0, 0))));
							for(int i = 0; i < 4; ++i)
							{
								Normals.push_back(normal.x); Normals.push_back(normal.y); Normals.push_back(normal.z);
							}

							vec2 coords(Tile % TILES_U, Tile / TILES_U);
							vec2 position = coords * GRID;
							Texcoords.push_back(position.x          + GAP); Texcoords.push_back(position.y          + GAP);
							Texcoords.push_back(position.x + GRID.x - GAP); Texcoords.push_back(position.y          + GAP);
							Texcoords.push_back(position.x          + GAP); Texcoords.push_back(position.y + GRID.y - GAP);
							Texcoords.push_back(position.x + GRID.x - GAP); Texcoords.push_back(position.y + GRID.y - GAP);

							if(dir == -1) {
								Elements.push_back(n+0); Elements.push_back(n+1); Elements.push_back(n+2);
								Elements.push_back(n+1); Elements.push_back(n+3); Elements.push_back(n+2);
							} else {
								Elements.push_back(n+0); Elements.push_back(n+2); Elements.push_back(n+1);
								Elements.push_back(n+1); Elements.push_back(n+2); Elements.push_back(n+3);
							}
							n += 4;
						}

					skip: dir *= -1; } while(dir > 0); }
				}
			}
			else
			{
				Clear();
				Debug::Pass("thread aborted for " + to_string(active));
				goto end;
			}
		}}}
		end:
		meshing = false;
	}

	void Clear()
	{
		Vertices.clear(); Normals.clear(); Texcoords.clear(); Elements.clear();
	}

	void Buffers(unsigned int id)
	{
		if(Elements.size() < 1)
		{
			Debug::Fail("Terrain buffering chunk " + to_string(id) + " aborted");
			return;
		}

		auto frm = Entity->Add<StorageForm>(id);

		glGenBuffers(1, &frm->Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
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

		frm->Texture = texture;
	}

	GLuint Texture()
	{
		Image image;
		bool result = image.loadFromFile("forms/textures/terrain.png");
		if(!result)
		{
			Debug::Fail("Terrain texture loading fail");
			return 0;
		}

		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());

		// cut tiles
		const Vector2u TILESIZE(image.getSize().x / TILES_U, image.getSize().y / TILES_V);
		Image tiles[TILES_U][TILES_V];
		for(uint u = 0; u < TILES_U; ++u)
		for(uint v = 0; v < TILES_V; ++v)
		{
			tiles[u][v].create(TILESIZE.x, TILESIZE.y);
			tiles[u][v].copy(image, 0, 0, IntRect(u * TILESIZE.x, v * TILESIZE.y, (u+1) * TILESIZE.x, (v+1) * TILESIZE.y), true);
		}

		// generate mipmaps
		Vector2u size = Vector2u(TILESIZE.x / 2, TILESIZE.y / 2);
		int level = 1;
		for(level; size.x > 1; size.x /= 2, size.y /= 2, ++level)
		{
			Image mipmap;
			mipmap.create(size.x * TILES_U, size.y * TILES_V);
			for(int u = 0; u < TILES_U; ++u)
			for(int v = 0; v < TILES_V; ++v)
			{
				shrink(tiles[u][v]);
				mipmap.copy(tiles[u][v], u * size.x, v * size.y); // later on copy alpha too
			}
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, mipmap.getSize().x, mipmap.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmap.getPixelsPtr());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level-1);

		glBindTexture(GL_TEXTURE_2D, 0);
		return id;
	}


	~ComponentTerrain()
	{
		glDeleteTextures(1, &texture);

		if(meshing)
		{
			meshing = false;
			task.get();
		}
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

	string vec_to_string(ivec3 a)
	{
		return (a.x >= 0 ? " " : "") + to_string(a.x) + (a.y >= 0 ? "  " : " ") + to_string(a.y) + (a.z >= 0 ? "  " : " ") + to_string(a.z);
	}

	void shrink(Image &image)
	{
		Vector2u size(image.getSize().x / 2, image.getSize().y / 2);

		Image half;
		half.create(size.x, size.y);
		for(uint x = 0; x < size.x; ++x)
		for(uint y = 0; y < size.y; ++y)
		{
			Color input[4];
			input[0] = image.getPixel(2*x + 0, 2*y + 0);
			input[1] = image.getPixel(2*x + 1, 2*y + 0);
			input[2] = image.getPixel(2*x + 0, 2*y + 1);
			input[3] = image.getPixel(2*x + 1, 2*y + 1);
			// average over them
			Color output = avg(input[0], input[1], input[2], input[3]);
			// store result in new pixel
			half.setPixel(x, y, output);
		}
		image = half;
	}
	Color avg(Color a, Color b, Color c, Color d)
	{
		return Color(((a.r+b.r+c.r+d.r)/4), ((a.g+b.g+c.g+d.g)/4), ((a.b+b.b+c.b+d.b)/4), ((a.a+b.a+c.a+d.a)/4));
	}
};