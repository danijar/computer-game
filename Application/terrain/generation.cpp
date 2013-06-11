#include "module.h"

#include <vector>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"


#define SEALEVEL std::min(15, CHUNK_SIZE.y)

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
			heightmaps.push_back(1.3f);
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
				float gradient = -y / heightmap / SEALEVEL + 1;
				if(gradient < 0) gradient = -(gradient * gradient);

				// rocks
				vector<float> rocks;
				if(AroundGroundlevel(y, heightmap, 1.0f, 1.2f))
					rocks.push_back(0.5f * amount_rocks * amount_rough * NoiseNormal(1.5f, sample));
				else if(AroundGroundlevel(y, heightmap, 0.8f, 1.0f))
					rocks.push_back(std::max(2.0f * amount_rocks * amount_rough * NoiseNormal(1.5f, sample), 0.0f));
				if(AroundGroundlevel(y, heightmap, 0.8f, 1.3f))
					rocks.push_back(0.4f * amount_rocks * amount_rough * (NoiseNormal(0.9f, sample) / 1.5f + 0.3f));
				rocks.push_back(0.1f * amount_rocks * NoiseNormal(2.0f, sample));
				float density = 0; for(auto i : rocks) density += i;

				// combination
				if(0.0f < gradient + density)
					Terrain->Blocks[x][y][z] = gradient > density ? rand() % 2 + 1 : 3;
			}

			// cover rocks with grass partly
			if(0 < NoiseNormal(0.5f, vec2(x, z)) && (0.5f < amount_rough || 0.5f < amount_rocks))
			{
				for(int y = CHUNK_SIZE.y - 1; y > 0 - 1; --y)
				{
					if(Terrain->Blocks[x][y][z])
					{
						Terrain->Blocks[x][y][z] = 2;
						break;
					}
				}
			}


		}
	}
}

float ModuleTerrain::NoiseNormal(float Zoom, vec2 Sample)
{
	return simplex(Zoom / SEALEVEL * Sample);
}

float ModuleTerrain::NoiseNormal(float Zoom, vec3 Sample)
{
	return simplex(Zoom / SEALEVEL * Sample);
}

float ModuleTerrain::NoiseLayered(float Zoom, vec2 Sample, int Layers)
{
	int primes[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 43 };
	Layers = std::min(Layers, (int)(sizeof(primes) / sizeof(float)));

	float value = 0;
	for(int i = 0; i < Layers; ++i)
	{
		float zoom = 0.1f * primes[i] + 1.0f;
		vec2 offset = vec2(1.0f / (primes[i + 0] * i), 1.0f / (primes[i + 1] * i));
		value += (-i / Layers + 1) * simplex(Zoom * zoom / SEALEVEL * (Sample + offset));
	}
	return value;
}

float ModuleTerrain::NoisePositive(float Zoom, vec2 Sample)
{
	return (simplex(Zoom / SEALEVEL * Sample) + 1) / 2;
}

float ModuleTerrain::NoiseSigmoid(float Zoom, vec2 Sample, float Shift, float Sharp)
{
	return 1 / (1 + (float)pow(10, (Sharp * (simplex(Zoom / SEALEVEL * Sample) - Shift))));
}

bool ModuleTerrain::AroundGroundlevel(int Sample, float Heightmap, float From, float To)
{
	return (Heightmap * SEALEVEL * From) < Sample && Sample < (Heightmap * SEALEVEL * To);
}
