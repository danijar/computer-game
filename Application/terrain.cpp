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
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
#include <GLM/gtx/norm.hpp>
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
	uint8_t focus;
	float distance;
	GLuint marker;

	void Init()
	{
		auto wld = Global->Add<StorageTerrain>("terrain");

		active = 0;
		meshing = false;

		focus = 0;
		distance = 0.f;

		Entity->Add<StorageText>(Entity->New())->Text = [=]
		{
			vec3 pos = Global->Get<StorageCamera>("camera")->Position;
			return "Chunk: " + vec_to_string(pos_chunk(ivec3(floor(pos.x), floor(pos.y), floor(pos.z))));
		};

		Entity->Add<StorageText>(Entity->New())->Text = [=]{ return "Focus: " + to_string(focus); };
		Entity->Add<StorageText>(Entity->New())->Text = [=]{ return "Distance: " + to_string(distance); };

		Listeners();
	}

	void Update()
	{
		auto wld = Global->Get<StorageTerrain>("terrain");
		auto stg = Global->Get<StorageSettings>("settings");
		auto cam = Global->Get<StorageCamera>("camera");

		const int distance = (unsigned int)((stg->Viewdistance / ((CHUNK.x + CHUNK.y + CHUNK.z) / 3.f)) / 5);
		const ivec2 camera   = ivec2(floor(cam->Position.x / CHUNK.x), floor(cam->Position.z / CHUNK.z));

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

		Selection();
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

	void Listeners()
	{
		Event->Listen("SystemInitialized", [=]{
			texture = Texture();

			auto cam = Global->Get<StorageCamera>("camera");
			cam->Position = vec3(0, CHUNK_Y, 0);
			cam->Angles = vec2(0.75, -0.25);

			marker = Marker();
		});
	}

	////////////////////////////////////////////////////////////
	// Chunks
	////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////
	// Blocks
	////////////////////////////////////////////////////////////

	uint8_t getBlock(ivec3 pos)
	{
		ivec3 block = pos_block(pos);
		unsigned int chunk = getChunk(pos_chunk(pos));
		if(chunk) return Entity->Get<StorageChunk>(chunk)->blocks[block.x][block.y][block.z];
		else      return 0;
	}
	/*
	void setBlock(ivec3 pos, uint8_t type) {
		ivec3 chunk = pos_chunk(pos);
		ivec3 block = pos_block(pos);
		auto cnk = Entity->Get<StorageChunk>(getChunk(chunk.x, chunk.y, chunk.z));
		cnk->blocks[block.x][block.y][block.z] = type;
		cnk->changed = true;
	}
	void removeBlock(ivec3 pos)
	{
		setBlock(pos, 0);
	}
	*/
	ivec3 pos_chunk(ivec3 pos)
	{
		ivec3 chunk;
		chunk.x = pos.x / CHUNK_X;
		chunk.y = pos.y / CHUNK_Y;
		chunk.z = pos.z / CHUNK_Z;
		return chunk;
	}
	ivec3 pos_block(ivec3 pos)
	{
		ivec3 block;
		block.x %= CHUNK_X;
		block.y %= CHUNK_Y;
		block.z %= CHUNK_Z;
		return block;
	}

	////////////////////////////////////////////////////////////
	// Generator (candidate for other component)
	////////////////////////////////////////////////////////////

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
				for(int y = 0; y < height; ++y) cnk->blocks[x][y][z] = rand() % 2 + 1;
		} }

		// mark origin of the world
		if(key.x == 0 && key.y == 0 && key.z == 0) for(int y = 0; y < CHUNK_Y; ++y) cnk->blocks[0][y][0] = 1;
	}

	////////////////////////////////////////////////////////////
	// Meshing
	////////////////////////////////////////////////////////////

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
					uint8_t tile = clamp((int)cnk->blocks[X][Y][Z], 0, TILES_U * TILES_V - 1);
					for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
						ivec3 neigh = shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

						if(inside(neigh, ivec3(0), CHUNK - 1))
							if(cnk->blocks[neigh.x][neigh.y][neigh.z])
								goto skip;

						{
							for(float i = 0; i <= 1; ++i)
							for(float j = 0; j <= 1; ++j)
							{
								vec3 vertex = vec3(X, Y, Z) + vec3(shift(dim, ivec3((dir+1)/2, i, j)));
								Vertices.push_back(vertex.x); Vertices.push_back(vertex.y); Vertices.push_back(vertex.z);
							}

							vec3 normal = normalize(vec3(shift(dim, ivec3(dir, 0, 0))));
							for(int i = 0; i < 4; ++i)
							{
								Normals.push_back(normal.x); Normals.push_back(normal.y); Normals.push_back(normal.z);
							}

							vec2 coords(tile % TILES_U, tile / TILES_U);
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
		Matrix(id);
	}

	void Matrix(unsigned int id)
	{
		auto tsf = Entity->Get<StorageTransform>(id);

		mat4 Scale      = scale    (mat4(1), tsf->Scale);
		mat4 Translate  = translate(mat4(1), tsf->Position);
		mat4 Rotate     = rotate   (mat4(1), tsf->Rotation.x, vec3(1, 0 ,0))
						* rotate   (mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
						* rotate   (mat4(1), tsf->Rotation.z, vec3(0, 0, 1));
		tsf->Matrix = Translate * Rotate * Scale;
	}

	bool inside(ivec3 Position, ivec3 Min, ivec3 Max)
	{
		if(Position.x < Min.x || Position.y < Min.y || Position.z < Min.z) return false;
		if(Position.x > Max.x || Position.y > Max.y || Position.z > Max.z) return false;
		return true;
	}

	inline ivec3 shift(int Dimension, ivec3 Vector)
	{
		if      (Dimension % 3 == 1) return ivec3(Vector.z, Vector.x, Vector.y);
		else if (Dimension % 3 == 2) return ivec3(Vector.y, Vector.z, Vector.x);
		else                         return Vector;
	}

	////////////////////////////////////////////////////////////
	// Texture
	////////////////////////////////////////////////////////////

	GLuint Texture()
	{
		Image image;
		bool result = image.loadFromFile("forms/textures/terrain.png");
		if(!result)
		{
			Debug::Fail("Terrain texture loading fail");
			return 0;
		}

		// generate texture
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
		int level;
		for(level = 0; size.x > 1 && size.y > 1; size.x /= 2, size.y /= 2)
		{
			level++;

			Image mipmap;
			mipmap.create(size.x * TILES_U, size.y * TILES_V);
			for(int u = 0; u < TILES_U; ++u)
			for(int v = 0; v < TILES_V; ++v)
			{
				shrink(tiles[u][v]);
				mipmap.copy(tiles[u][v], u * size.x, v * size.y, IntRect(0, 0, 0, 0), true);
			}
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, mipmap.getSize().x, mipmap.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmap.getPixelsPtr());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);

		glBindTexture(GL_TEXTURE_2D, 0);
		return id;
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
			half.setPixel(x, y, avg(input[0], input[1], input[2], input[3]));
		}
		image = half;
	}

	Color avg(Color a, Color b, Color c, Color d)
	{
		return Color(((a.r+b.r+c.r+d.r)/4), ((a.g+b.g+c.g+d.g)/4), ((a.b+b.b+c.b+d.b)/4), ((a.a+b.a+c.a+d.a)/4));
	}

	////////////////////////////////////////////////////////////
	// Selection
	////////////////////////////////////////////////////////////

	vec3 destination;
	pair<ivec3, uint8_t> Selection()
	{
		auto cam = Global->Get<StorageCamera>("camera");
		Vector2u size = Global->Get<RenderWindow>("window")->getSize();

		vec3 origin = cam->Position;
		//vec3 origin      = unProject(vec3(size.x/2, size.y/2, 0.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));
		vec3 destination = unProject(vec3(size.x/2, size.y/2, 1.f), cam->View, cam->Projection, vec4(0, 0, size.x, size.y));

		const float reach = 32.f;

		int x = (int)floor(origin.x);
		int y = (int)floor(origin.y);
		int z = (int)floor(origin.z);

		float dx = destination.x - origin.x;
		float dy = destination.y - origin.y;
		float dz = destination.z - origin.z;

		int stepX = signum(dx);
		int stepY = signum(dy);
		int stepZ = signum(dz);

		float tMaxX = intbound(origin.x, dx);
		float tMaxY = intbound(origin.y, dy);
		float tMaxZ = intbound(origin.z, dz);

		float tDeltaX = stepX/dx;
		float tDeltaY = stepY/dy;
		float tDeltaZ = stepZ/dz;

		uint8_t material = 0;
		while(!material)
		{
			if (tMaxX < tMaxY)
			{
				if (tMaxX < tMaxZ)
				{
					if (tMaxX > reach) break;
					x += stepX;
					tMaxX += tDeltaX;
				}
				else
				{
					z += stepZ;
					tMaxZ += tDeltaZ;
				}
			}
			else
			{
				if (tMaxY < tMaxZ)
				{
					if (tMaxY > reach) break;
					y += stepY;
					tMaxY += tDeltaY;
				}
				else
				{
					if (tMaxZ > reach) break;
					z += stepZ;
					tMaxZ += tDeltaZ;
				}
			}
			material = getBlock(ivec3(x, y, z));
		}

		this->focus = material;
		this->distance = glm::length2(vec3(x, y, z) - origin);

		auto tsf = Entity->Get<StorageTransform>(marker);
		tsf->Position = vec3(x, y, z);
		Matrix(marker);

		return make_pair(ivec3(x, y, z), material);
	}

	// Find the smallest positive t such that s+t*ds is an integer.
	float intbound(float s, float ds)
	{
		if (ds < 0) {
			return intbound(-s, -ds);
		} else {
			s = mod(s, 1.f);
			return (1-s) / ds;
		}
	}

	int signum(float x)
	{
		return x > 0 ? 1 : x < 0 ? -1 : 0;
	}


	string vec_to_string(vec3 a)
	{
		return "X " + to_string(a.x) + " Y " + to_string(a.y) + " Z " + to_string(a.z);
	}
	string vec_to_string(ivec3 a)
	{
		return "X " + to_string(a.x) + " Y " + to_string(a.y) + " Z " + to_string(a.z);
	}

	unsigned int Marker()
	{
		unsigned int id = Entity->New();
		auto frm = Entity->Add<StorageForm>(id);
		auto tsf = Entity->Add<StorageTransform>(id);

		const GLfloat Vertices[]  = {0.,0.,1.,1.,0.,1.,1.,1.,1.,0.,1.,1.,0.,1.,1.,1.,1.,1.,1.,1.,0.,0.,1.,0.,1.,0.,0.,0.,0.,0.,0.,1.,0.,1.,1.,0.,0.,0.,0.,1.,0.,0.,1.,0.,1.,0.,0.,1.,0.,0.,0.,0.,0.,1.,0.,1.,1.,0.,1.,0.,1.,0.,1.,1.,0.,0.,1.,1.,0.,1.,1.,1.};
		glGenBuffers(1, &frm->Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
		glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), Vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		const GLfloat Normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
		glGenBuffers(1, &frm->Normals);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), Normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		const GLfloat Texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
		glGenBuffers(1, &frm->Texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GLfloat), Texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		const GLuint  Elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};
		glGenBuffers(1, &frm->Elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), Elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		Image image;
		bool result = image.loadFromFile("forms/textures/magic.jpg");
		auto size = image.getSize();
		glGenTextures(1, &frm->Texture);
		glBindTexture(GL_TEXTURE_2D, frm->Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result ? size.x : 1, result ? size.y : 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, result ? image.getPixelsPtr() : nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		tsf->Scale = vec3(1.0, 1.0, 1.0);
		Matrix(id);

		return id;
	}
};