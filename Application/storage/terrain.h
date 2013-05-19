#pragma once

#include <unordered_map>
#include <stdint.h>
#include <GLM/glm.hpp>


#define CHUNK 16

struct Terrain
{
	Terrain() : Changed(false) { memset(Blocks, 0, sizeof Blocks); }

	uint8_t Blocks[CHUNK][CHUNK][CHUNK];
	glm::ivec3 Key;
	bool Changed;
};
