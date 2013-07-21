#pragma once

#include <string>
#include <unordered_map>
#include <stdint.h>
#include <GLM/glm.hpp>


#define CHUNK_SIZE ivec3(16, 16, 16)

struct Terrain
{
	Terrain() : Changed(false) { memset(Blocks, 0, sizeof Blocks); }

	uint8_t Blocks[16][32][16];

	enum { ROCKS, PLAIN, ROUGH, VEGETATION };
	float Details[16][16][4];

	glm::ivec3 Key;
	bool Changed;
};
