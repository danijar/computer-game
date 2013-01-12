#pragma once

#include "system.h"

#include <map>
#include <array>
using namespace std;
#include <SFML\System\Vector3.hpp>
using namespace sf;


#define CHUNK_X 32
#define CHUNK_Y 32
#define CHUNK_Z 32

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
	map<array<int, 3>, int> chunks;
};