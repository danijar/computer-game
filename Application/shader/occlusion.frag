#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D noise_tex;

uniform float totStrength = 0.8;
uniform float strength = 0.05;
uniform float offset = 10.0;
uniform float falloff = 0.005;
uniform float rad = 0.001;
#define SAMPLES 8
const float invSamples = - 1.0 / 8;

void main()
{
	const vec3 pSphere[8] = vec3[](vec3(0.24710192, 0.6445882, 0.033550154),vec3(0.00991752, -0.21947019, 0.7196721),vec3(0.25109035, -0.1787317, -0.011580509),vec3(-0.08781511, 0.44514698, 0.56647956),vec3(-0.011737816, -0.0643377, 0.16030222),vec3(0.035941467, 0.04990871, -0.46533614),vec3(-0.058801126, 0.7347013, -0.25399926),vec3(-0.24799341, -0.022052078, -0.13399573));

	vec3 fres = normalize((texture2D(noise_tex, coord*offset).xyz*2.0) - vec3(1.0));
 
	vec4 currentPixelSample = texture2D(normal_tex, coord);
 
	float currentPixelDepth = (texture2D(position_tex, coord).z + 5.0 )/ 1000.0f;
 
	// current fragment coords in screen space
	vec3 ep = vec3(coord.xy,currentPixelDepth);
	// get the normal of current fragment
	vec3 norm = currentPixelSample.xyz;
 
	float bl = 0.0;
	// adjust for the depth ( not shure if this is good..)
	float radD = rad/currentPixelDepth;
 
	vec3 ray, se, occNorm;
	float occluderDepth, depthDifference, normDiff;
 
	for(int i=0; i<SAMPLES;++i)
	{
		// get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
		ray = radD*reflect(pSphere[i],fres);
 
		// if the ray is outside the hemisphere then change direction
		se = ep + sign(dot(ray,norm) )*ray;
 
		// get the depth of the occluder fragment
		vec4 occluderFragment = texture2D(normal_tex, se.xy);
		occluderFragment.a = texture2D(position_tex, se.xy).z / 1000.0f;
 
		// get the normal of the occluder fragment
		occNorm = occluderFragment.xyz;
 
		// if depthDifference is negative = occluder is behind current fragment
		depthDifference = currentPixelDepth-occluderFragment.a;
 
		// calculate the difference between the normals as a weight
 
		normDiff = (1.0-dot(occNorm,norm));
		// the falloff equation, starts at falloff and is kind of 1/x^2 falling
		bl += step(falloff,depthDifference)*normDiff*(1.0-smoothstep(falloff,strength,depthDifference));
	}
 
	// output the result
	float ao = totStrength*bl*invSamples + 1;

	image = vec4(vec3(ao), 1.0);
	//image = vec4(texture2D(image_tex, coord).xyz * ao, 1.0);
	//image = texture2D(image_tex, coord);
 }
