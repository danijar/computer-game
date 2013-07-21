#include "module.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
using namespace glm;


float ModuleTerrain::NoiseLayered(float Zoom, vec2 Sample, int Layers)
{
	int primes[] = { 0, 1, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 43 };
	Layers = min(Layers, (int)(sizeof(primes) / sizeof(float)));

	float value = 0;
	for(int i = 0; i < Layers; ++i)
	{
		float zoom = 0.1f * primes[i] + 1.0f;
		value += simplex(Zoom * zoom * Sample) / i;
	}
	return value;
}

float ModuleTerrain::NoisePositive(float Zoom, vec2 Sample)
{
	return (simplex(Zoom * Sample) + 1) / 2;
}

float ModuleTerrain::NoiseSigmoid(float Zoom, vec2 Sample, float Shift, float Sharp)
{
	return 1 / (1 + (float)pow(10, (Sharp * (simplex(Zoom * Sample) - Shift))));
}

float ModuleTerrain::FractionalBrownianMotion(float Zoom, vec3 Sample, int Octave, float Lacunarity, float Gain)
{
	float noiseSum     = 0.0f;
	float amplitude    = 1.0f;
	float amplitudeSum = 0.0f;
	
	vec3 sample = Zoom * Sample;
	
	for( int i = 0; i < Octave; ++i )
	{
		noiseSum     += amplitude * simplex(sample);
		amplitudeSum += amplitude;
		
		amplitude *= Gain;
		sample    *= Lacunarity;
	}
	
	return noiseSum / amplitudeSum;
}
