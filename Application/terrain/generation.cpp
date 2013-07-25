#include "module.h"

#include <vector>
#include <cstdlib>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"


void ModuleTerrain::Generate(Terrain *Terrain)
{
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;
	
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	for(int z = 0; z < CHUNK_SIZE.z; ++z)
	{
		// two dimensional
		vec2 sample(offset.x + x, offset.z + z);

		// area characteristics
		const int WORLD_HEIGHT = 20 + 12 * simplex(0.007f * sample);

		float   amount_rocks      = NoiseSigmoid(0.020f, sample, -0.5f, 5.0f),
		        amount_plain      = NoiseSigmoid(0.013f, sample, -0.3f, 10.0f),
		        amount_rough      = 1 - amount_plain,
		        amount_vegetation = simplex(0.033f * sample) / 2.0f + 0.5f;

		Terrain->Details[x][z][Terrain::ROCKS]      = amount_rocks;
		Terrain->Details[x][z][Terrain::PLAIN]      = amount_plain;
		Terrain->Details[x][z][Terrain::ROUGH]      = amount_rough;
		Terrain->Details[x][z][Terrain::VEGETATION] = amount_vegetation;

		// heightmap
		vector<float> heightmaps;
		heightmaps.push_back(0.25f * amount_rough * simplex(0.0267f * sample));
		heightmaps.push_back(0.07f * amount_rough * simplex(0.1000f * sample));
		float heightmap = 0; for(auto i : heightmaps) heightmap += i;

		for(int y = 0; y < CHUNK_SIZE.y; ++y)
		{
			// three dimensional
			vec3 sample(offset.x + x, offset.y + y, offset.z + z);
			float height = sample.y / WORLD_HEIGHT;

			// heightmap
			float gradient = -1.0f / (height < heightmap ? heightmap + 1 : 1 - heightmap) * (height - heightmap);
			if(gradient < 0) gradient = -(gradient * gradient);
			//float gradient = -sample.y / (heightmap + 1.3f) / 16.0f;

			// rocks
			vector<float> rocks;
			if(heightmap < height && height < heightmap + 0.1875f)
				rocks.push_back(0.5f * amount_rocks * amount_rough * simplex(0.1f * sample));
			else if(heightmap - 0.1875f < height && height < heightmap)
				rocks.push_back(std::max(2.0f * amount_rocks * amount_rough * simplex(0.1f * sample), 0.0f));
			if(heightmap - 0.1875f < height && height < heightmap + 0.2813f)
				rocks.push_back(0.4f * amount_rocks * amount_rough * (simplex(0.06f * sample) / 1.5f + 0.3f));
			rocks.push_back(0.1f * amount_rocks * simplex(0.1333f * sample));
			float rock = 0; for(auto i : rocks) rock += i;

			// combination
			if(0.0f < gradient + rock)
			{
				uint8_t type = 0;

				type = rand() % 2 > 0 ? 1 : 2; // grass or dirt

				if(rock > gradient)
				{
					float ores = FractionalBrownianMotion(0.07f, sample, 6, 2, 0.5f);
					if(ores > 0.4f)
						type = rand() % 2 > 0 ? 8 : 3; // coal or stone
					else
						type = 3; // stone
				}

				Terrain->Blocks[x][y][z] = type;
			}
		}
	}

	GenerateGrass(Terrain);
	GenerateTrees(Terrain);
	// ...
}

void ModuleTerrain::GenerateGrass(Terrain *Terrain)
{
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;

	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	for(int z = 0; z < CHUNK_SIZE.z; ++z)
	{
		vec2 sample(offset.x + x, offset.z + z);

		float amount = 0;
		amount += Terrain->Details[x][z][Terrain::VEGETATION];
		amount += 0.5f * Terrain->Details[x][z][Terrain::ROCKS];
		amount += simplex(0.05f * sample);

		if(0.2f < amount)
		{
			for(int y = CHUNK_SIZE.y - 1; y > 0 - 1; --y)
			{
				uint8_t type = Terrain->Blocks[x][y][z];
				if(type)
				{
					Terrain->Blocks[x][y][z] = 2;
					break;
				}
			}
		}
	}
}

void ModuleTerrain::GenerateTrees(Terrain *Terrain)
{
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;

	// spread tree locations
	vector<ivec3> trees;
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	for(int z = 0; z < CHUNK_SIZE.z; ++z)
	{
		vec2 sample(offset.x + x, offset.z + z);
		if(0.3f < Terrain->Details[x][z][Terrain::VEGETATION])
		{
			srand(hash<float>()(sample.x) + 17 * hash<float>()(sample.y));
			if(0.1f > (rand() % 1000) / 1000.0f)
			{
				for(int y = CHUNK_SIZE.y - 1; y > 0 - 1; --y)
				{
					if(Terrain->Blocks[x][y][z] == 2)
					{
						trees.push_back(ivec3(x, y + 1, z));
						break;
					}
				}
			}
		}
	}

	for(auto i : trees)
	{
		int height = 4 + rand() % 3,
		    radius = clamp(height / 2 - 1 + (rand() % 1), 1, 3);

		// check for enough space
		if(!(radius < i.x && i.x < CHUNK_SIZE.x - radius)) continue;
		if(!(radius < i.z && i.z < CHUNK_SIZE.z - radius)) continue;
		if(!(0      < i.y && i.y < CHUNK_SIZE.y - height)) continue;
		bool intersection = false;
		for(int x = -radius; x < radius && !intersection; ++x)
		for(int y = -radius; y < radius && !intersection; ++y)
		for(int z = -radius; z < radius && !intersection; ++z)
			if(Terrain->Blocks[i.x + x][i.y + height - radius + y][i.z + z])
				intersection = true;
		if(intersection) continue;

		// place trunk
		for(int y = 0; y < height - 2; ++y)
			Terrain->Blocks[i.x][i.y + y][i.z] = 5;
	
		// place leafes
		for(int x = -radius; x <= radius; ++x)
		for(int y = -radius; y <= radius; ++y)
		for(int z = -radius; z <= radius; ++z)
			if(x * x + y * y + z * z <= radius * radius + 1)
				if(!Terrain->Blocks[i.x + x][i.y + height - radius + y][i.z + z])
					Terrain->Blocks[i.x + x][i.y + height - radius + y][i.z + z] = 7;
	}
}
