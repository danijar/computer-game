#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace std;
using namespace glm;

#include "terrain.h"


void ModuleTerrain::Generate(Terrain *Terrain)
{
	/*
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	{
		const float i = Terrain->Key.x + (float)x / CHUNK_SIZE.x;
		for(int z = 0; z < CHUNK_SIZE.z; ++z)
		{
			const float j = Terrain->Key.z + (float)z / CHUNK_SIZE.z;

			double height_bias = 0.30;
			double height_base = 0.50 * (simplex(0.2f * vec2(i, j)) + 1) / 2;
			double height_fine = 0.20 * (simplex(1.5f * vec2(i, j)) + 1) / 2;
			int height = (int)((height_bias + height_base + height_fine) * CHUNK_SIZE.y);
			for(int y = 0; y < height && y < CHUNK_SIZE.y; ++y) Terrain->Blocks[x][y][z] = rand() % 2 + 1;
		}
	}
	*/

	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	{
		const float i = Terrain->Key.x + (float)x / CHUNK_SIZE.x;
		for(int z = 0; z < CHUNK_SIZE.z; ++z)
		{
			const float j = Terrain->Key.z + (float)z / CHUNK_SIZE.z;

			double height_bias = 0.30;
			double height_base = 0.50 * (simplex(0.2f * vec2(i, j)) + 1) / 2;
			double height_fine = 0.20 * (simplex(1.5f * vec2(i, j)) + 1) / 2;
			int height = (int)((height_bias + height_base + height_fine) * CHUNK_SIZE.y);
			for(int y = 0; y < height && y < CHUNK_SIZE.y; ++y) Terrain->Blocks[x][y][z] = rand() % 2 + 1;
		}
	}
}
