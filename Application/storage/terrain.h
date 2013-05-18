#pragma once

#include <unordered_map>
#include <stdint.h>
#include <GLM/glm.hpp>

#include <iostream>


#define CHUNK 8

struct Terrain
{
	Terrain() : Changed(false) { memset(Blocks, 0, sizeof Blocks); }

	uint8_t Blocks[CHUNK][CHUNK][CHUNK];
	glm::ivec3 Chunk;
	bool Changed;
};
