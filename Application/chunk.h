#pragma once

#include "system.h"

#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

struct StorageChunk : public Storage
{
	uint8_t blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	bool changed;
	int elements; // number of triangles (or vertices?) for the renderer

	StorageChunk() : changed(1)
	{
		memset(blocks, 0, sizeof blocks);
	}

	uint8_t Get(int x, int y, int z) {
		return blocks[x][y][z];
	}

	void Set(int x, int y, int z, uint8_t type) {
		blocks[x][y][z] = type;
		changed = true;
	}
};