#include "module.h"

#include <vector>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"


void ModuleTerrain::Generate(Terrain *Terrain)
{
	const int maxground = std::min(15, CHUNK_SIZE.y);
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	{
		for(int z = 0; z < CHUNK_SIZE.z; ++z)
		{
			////////////////////////////////////////////////////////////
			// two dimensional
			////////////////////////////////////////////////////////////

			// area characteristics
			float amount_rocks = 1 / (1 + (float)pow(10000, simplex(0.30f / maxground * vec2(offset.x + x, offset.z + z)) + 0.50f));
			float amount_plain = 1 / (1 + (float)pow(10000, simplex(0.10f / maxground * vec2(offset.x + x, offset.z + z)) - 0.20f));
			float amount_rough = 1 - amount_plain;

			// heightmap
			vector<float> heightmaps;
			heightmaps.push_back(0.3f);
			heightmaps.push_back(0.5f * amount_rough * (simplex(0.4f / maxground * vec2(offset.x + x, offset.z + z))) / 2 + 0.5f);
			heightmaps.push_back(0.2f * amount_rough * (simplex(1.5f / maxground * vec2(offset.x + x, offset.z + z))) / 2 + 0.5f);
			float heightmap = 0; for(auto i : heightmaps) heightmap += i;

			for(int y = 0; y < CHUNK_SIZE.y; ++y)
			{
				////////////////////////////////////////////////////////////
				// three dimensional
				////////////////////////////////////////////////////////////

				// heightmap
				float gradient = -y / heightmap / maxground + 1;
				if(gradient < 0) gradient = -(gradient * gradient);

				// rocks
				vector<float> rocks;
				if(y > heightmap * maxground && y < heightmap * maxground * 1.2f)
					rocks.push_back(0.5f * amount_rocks * amount_rough * simplex(1.5f / maxground * vec3(offset + ivec3(x, y, z))));
				if(y > heightmap * maxground * 0.9f && y < heightmap * maxground * 1.3f)
					rocks.push_back(0.4f * amount_rocks * amount_rough * (simplex(0.9f / maxground * vec3(offset.x + x, offset.y + y, offset.z + z)) / 1.5f + 0.3f));
				rocks.push_back(0.2f * amount_rocks * (simplex(2.0f / maxground * vec3(offset.x + x, offset.y + y, offset.z + z)) / 2));
				float density = 0; for(auto i : rocks) density += i;

				// combination
				if(0.0f < gradient + density)
					Terrain->Blocks[x][y][z] = gradient > 0 ? rand() % 2 + 1 : 3;
			}
		}
	}
}
