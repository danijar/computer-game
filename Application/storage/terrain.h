#pragma once

#include <unordered_map>
#include <stdint.h>
#include <GLM/glm.hpp>

#define CHUNK_X 32
#define CHUNK_Y 32
#define CHUNK_Z 32
#define CHUNK ivec3(CHUNK_X, CHUNK_Y, CHUNK_Z)

struct Terrain
{
	Terrain() : Changed(false) { memset(Blocks, 0, sizeof Blocks); }

	uint8_t Blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	glm::ivec3 Chunk;
	bool Changed;
};
