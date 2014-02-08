#pragma once

#include <string>
#include <unordered_map>
#include <stdint.h>
#include <dependency/glm/glm.hpp>

#define CHUNK_LENGTH 32
#define CHUNK_SIZE ivec3(CHUNK_LENGTH, CHUNK_LENGTH, CHUNK_LENGTH)


struct Terrain
{
	Terrain() : Changed(false) { memset(Blocks, 0, sizeof Blocks); }

	uint8_t Blocks[CHUNK_LENGTH][CHUNK_LENGTH][CHUNK_LENGTH];

	enum { ROCKS, PLAIN, ROUGH, VEGETATION };
	float Details[CHUNK_LENGTH][CHUNK_LENGTH][4];

	glm::ivec3 Key;
	bool Changed;
};
