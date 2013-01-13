#pragma once

#include "system.h"

#include <unordered_map>
using namespace std;
#include <GLM/glm.hpp>
using namespace glm;


#define CHUNK_X 32
#define CHUNK_Y 32
#define CHUNK_Z 32
#define CHUNK ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z)

struct StorageChunk : public Storage
{
	StorageChunk() : changed(true)
	{
		memset(blocks, false, sizeof blocks);
	}

	bool blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	bool changed;
};

struct StorageTerrain : public Storage
{
	struct traits {
		size_t operator()(const ivec3& k)const {
			return hash<int>()(k.x) ^ hash<int>()(k.y) ^ hash<int>()(k.z);
		}
		bool operator()(const ivec3& a, const ivec3& b)const {
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};

	unordered_map<ivec3, unsigned int, traits, traits> chunks;
};