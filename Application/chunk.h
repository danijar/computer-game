#pragma once

#include "system.h"


#define CHUNK_X 64
#define CHUNK_Y 16
#define CHUNK_Z 64

struct StorageChunk : public Storage
{
	StorageChunk() : changed(1)
	{
		memset(blocks, 0, sizeof blocks);
	}

	bool blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	bool changed;
};