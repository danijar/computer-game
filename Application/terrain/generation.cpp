#include "module.h"

#include <vector>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"


#define MAX_GROUNDLEVEL std::min(15, CHUNK_SIZE.y)

void ModuleTerrain::Generate(Terrain *Terrain)
{
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	{
		for(int z = 0; z < CHUNK_SIZE.z; ++z)
		{
			////////////////////////////////////////////////////////////
			// two dimensional
			////////////////////////////////////////////////////////////
			vec2 sample = vec2(offset.x + x, offset.z + z);

			// area characteristics
			float amount_rocks = NoiseSigmoid(0.3f, sample, -0.5f, 5.0f);
			float amount_plain = NoiseSigmoid(0.2f, sample, -0.3f, 10.0f);
			float amount_rough = 1 - amount_plain;

			// heightmap
			vector<float> heightmaps;
			heightmaps.push_back(1.3f); // should be only 0.3f
			heightmaps.push_back(0.25f * amount_rough * NoiseNormal(0.4f, sample));
			heightmaps.push_back(0.10f * amount_rough * NoiseNormal(1.5f, sample));
			float heightmap = 0; for(auto i : heightmaps) heightmap += i;

			for(int y = 0; y < CHUNK_SIZE.y; ++y)
			{
				////////////////////////////////////////////////////////////
				// three dimensional
				////////////////////////////////////////////////////////////
				vec3 sample = vec3(offset.x + x, offset.y + y, offset.z + z);

				// heightmap
				float gradient = -y / heightmap / MAX_GROUNDLEVEL + 1;
				if(gradient < 0) gradient = -(gradient * gradient);

				// rocks
				vector<float> rocks;
				if(AroundGroundlevel(y, heightmap, 1.0f, 1.2f))
					rocks.push_back(0.5f * amount_rocks * amount_rough * NoiseNormal(1.5f, sample));
				if(AroundGroundlevel(y, heightmap, 0.9f, 1.3f))
					rocks.push_back(0.4f * amount_rocks * amount_rough * (NoiseNormal(0.9f, sample) / 1.5f + 0.3f));
				rocks.push_back(0.1f * amount_rocks * NoiseNormal(2.0f, sample));
				float density = 0; for(auto i : rocks) density += i;

				// combination
				if(0.0f < gradient + density)
					Terrain->Blocks[x][y][z] = gradient > 0 ? rand() % 2 + 1 : 3;
			}
		}
	}
}

float ModuleTerrain::NoiseNormal(float Zoom, glm::vec2 Sample)
{
	return simplex(Zoom / MAX_GROUNDLEVEL * Sample);
}

float ModuleTerrain::NoiseNormal(float Zoom, glm::vec3 Sample)
{
	return simplex(Zoom / MAX_GROUNDLEVEL * Sample);
}

float ModuleTerrain::NoisePositive(float Zoom, glm::vec2 Sample)
{
	return (simplex(Zoom / MAX_GROUNDLEVEL * Sample) + 1) / 2;
}

float ModuleTerrain::NoiseSigmoid(float Zoom, glm::vec2 Sample, float Shift, float Sharp)
{
	return 1 / (1 + (float)pow(10, (Sharp * (simplex(Zoom / MAX_GROUNDLEVEL * Sample) - Shift))));
}

bool ModuleTerrain::AroundGroundlevel(int Sample, float Heightmap, float From, float To)
{
	return (Heightmap * MAX_GROUNDLEVEL * From) < Sample && Sample < (Heightmap * MAX_GROUNDLEVEL * To);
}
