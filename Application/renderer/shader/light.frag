#version 330

in vec2 coord;
out vec4 image;

uniform int type = 0;

uniform sampler2D positions;
uniform sampler2D normals;
uniform vec3 light;
uniform vec3 color;
uniform float radius;
uniform float intensity = 1.0;


void main()
{
	if(type == 0) // directional light
	{
		vec3 normal = texture2D(normals, coord).xyz;
		float fraction = max(dot(normalize(light), normal) / 2.0 + 0.5, 0);
		image.xyz = intensity * color * fraction;
	}
	else if(type == 1) // point light
	{
		vec3 pixel = texture2D(positions, coord).xyz;
		vec3 normal = texture2D(normals, coord).xyz;
		float dist = max(distance(pixel, light), 1);
		float magnitude = 1 / pow(dist / radius + 1, 2);
		float cutoff = 0.4;
		float attenuation = clamp((magnitude - cutoff) / (1 - cutoff), 0, 1);
		float fraction = clamp(dot(normalize(light - pixel), normal), -1, 1);

		// attenuation = attenuation > 0 ? 0.5 : 0; // show radius
		image.xyz = intensity * color * attenuation * max(fraction, 0.2);
	}
	else if(type == 2) // spot light
	{

	}
}
