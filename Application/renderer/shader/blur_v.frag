#version 330

in vec2 coord;
out vec3 result;

uniform sampler2D image_tex;
uniform vec2 frame_size;
uniform int radius = 1;


vec3 fetch(in float u, in float v)
{
	return texture2D(image_tex, coord + vec2(u, v) / frame_size).rgb;
}

void main()
{
	vec3 sum = vec3(0); int n = 0;
	for(int v = -radius; v <= radius; ++v)
	{
		sum += fetch(0, v);
		n++;
	}
	result = sum / n;
}
