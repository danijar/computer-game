#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;


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
	Layers = min(Layers, (int)(sizeof(primes) / sizeof(float)));

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
