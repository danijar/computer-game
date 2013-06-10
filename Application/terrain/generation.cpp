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
			// two dimensional
			vector<float> layers;
			layers.push_back(0.3f);
			layers.push_back(0.5f * (simplex(0.375f / maxground * vec2(offset.x + x, offset.z + z))) / 2 + 0.5f);
			layers.push_back(0.2f * (simplex(1.500f / maxground * vec2(offset.x + x, offset.z + z))) / 2 + 0.5f);
			float height = 0; for(auto i : layers) height += i;

			for(int y = 0; y < CHUNK_SIZE.y; ++y)
			{
				// three dimensional
				float gradient = -y / height / maxground + 1;
				float density = 0.3f * (simplex(2.000f / maxground * vec3(offset.x + x, offset.y + y, offset.z + z)) / 1.5f + 0.3f);

				if(0.0f < gradient + density)
					Terrain->Blocks[x][y][z] = rand() % 2 + 1;
			}
		}
	}
}
