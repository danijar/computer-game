#include "module.h"

#include <vector>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;
using namespace std;

#include "terrain.h"


void ModuleTerrain::Generate(Terrain *Terrain)
{
	const int maxheight = std::min(15, CHUNK_SIZE.y);
	const ivec3 offset = Terrain->Key * CHUNK_SIZE;
	for(int x = 0; x < CHUNK_SIZE.x; ++x)
	{
		for(int z = 0; z < CHUNK_SIZE.z; ++z)
		{
			// two dimensional
			vector<float> layers;
			layers.push_back(0.3f);
			layers.push_back(0.5f * (.5f * simplex(0.375f / maxheight * vec2(offset.x + x, offset.z + z)) + .5f));
			layers.push_back(0.2f * (.5f * simplex(1.500f / maxheight * vec2(offset.x + x, offset.z + z)) + .5f));
			float sum = 0; for(auto i : layers) sum += i;
			int height = (int)(sum * maxheight);

			for(int y = 0; y < CHUNK_SIZE.y; ++y)
			{
				// three dimensional
				if(y < height)
					Terrain->Blocks[x][y][z] = rand() % 2 + 1;
			}
		}
	}
}
