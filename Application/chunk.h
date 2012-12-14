#pragma once

#include "system.h"

#include "form.h"

#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

struct StorageChunk : public Storage, public Form
{
	uint8_t blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
};