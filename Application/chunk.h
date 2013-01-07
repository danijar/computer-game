#pragma once

#include "system.h"


#define CHUNK_X 32
#define CHUNK_Y  8
#define CHUNK_Z 32

struct StorageChunk : public Storage
{
	StorageChunk() : changed(1)
	{
		memset(blocks, 0, sizeof blocks);
	}

	bool blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	bool changed;
};