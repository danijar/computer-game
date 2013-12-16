#include "module.h"

#include <dependency/glm/glm.hpp>
#include <dependency/glm/gtc/noise.hpp>
using namespace glm;


float ModuleTerrain::NoisePositive(float Zoom, vec2 Sample)
{
	return (simplex(Zoom * Sample) + 1.0f) / 2.0f;
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
