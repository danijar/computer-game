#include "module.h"

#include <vector>
#include <cstdlib>
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
			float amount_rocks      = NoiseSigmoid(0.3f, sample, -0.5f, 5.0f),
			      amount_plain      = NoiseSigmoid(0.2f, sample, -0.3f, 10.0f),
			      amount_rough      = 1 - amount_plain,
				  amount_vegetation = NoiseLayered(0.5f, sample, 5) / 2 + 0.5f;

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
			if(0.2f < amount_vegetation)
			{
				for(int y = CHUNK_SIZE.y - 1; y > 0 - 1; --y)
				{
					uint8_t type = Terrain->Blocks[x][y][z];
					if(type == 1 || type == 3)
					{
						Terrain->Blocks[x][y][z] = 2;
						break;
					}
				}
			}

			// place trees
			vector<ivec3> trees;
			if(0.4f < amount_vegetation - NoisePositive(0.2f, sample))
			{
				srand(hash<float>()(sample.x) + 17 * hash<float>()(sample.y));
				if(0.05f > (rand() % 1000) / 1000.0f)
				{
					for(int y = CHUNK_SIZE.y - 1; y > 0 - 1; --y)
					{
						if(Terrain->Blocks[x][y][z] == 2)
						{
							ivec3 position(x, y + 1, z);
							if(StructureTree(Terrain, position, 6, true))
								trees.push_back(position);
							break;
						}
					}
				}
			}
			for(auto i : trees)
				StructureTree(Terrain, i, 6);

			// add other structures
			// ...

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
	int primes[] = { 0, 1, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 43 };
	Layers = std::min(Layers, (int)(sizeof(primes) / sizeof(float)));

	float value = 0;
	for(int i = 0; i < Layers; ++i)
	{
		float zoom = 0.1f * primes[i] + 1.0f;
		value += simplex(Zoom * zoom / SEALEVEL * Sample) / i;
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

bool ModuleTerrain::StructureTree(Terrain *Terrain, ivec3 Position, int Size, bool Simulate)
{
	int height = Size,
	    radius = Size / 2 - 1;

	// test for space
	if(!Inside(Position, ivec3(radius, 0, radius), CHUNK_SIZE - ivec3(radius, height, radius))) return false;
	for(int x = -radius; x < radius; ++x)
	for(int y = -radius; y < radius; ++y)
	for(int z = -radius; z < radius; ++z)
		if(Terrain->Blocks[Position.x + x][Position.y + height - radius + y][Position.z + z])
			return false;

	if(Simulate) return true;

	// place trunk
	for(int y = 0; y < height - 2; ++y)
		Terrain->Blocks[Position.x][Position.y + y][Position.z] = 5;
	
	// place leafes
	for(int x = -radius; x <= radius; ++x)
	for(int y = -radius; y <= radius; ++y)
	for(int z = -radius; z <= radius; ++z)
		if(x * x + y * y + z * z <= radius * radius)
			if(!Terrain->Blocks[Position.x + x][Position.y + height - radius + y][Position.z + z])
				Terrain->Blocks[Position.x + x][Position.y + height - radius + y][Position.z + z] = 7;

	return true;
}
