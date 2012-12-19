#pragma once

#include "system.h"


#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

struct StorageChunk : public Storage
{
	bool blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	bool changed;

	StorageChunk() : changed(1)
	{
		memset(blocks, 0, sizeof blocks);
	}
};