#version 330

in vec2 coord;
out vec4 result;

uniform sampler2D image_tex;
uniform vec2 frameBufSize;
uniform int radius = 3;


vec4 fetch(in float u, in float v)
{
	return texture2D(image_tex, coord + vec2(u, v) / frameBufSize);
}

void main()
{
	vec4 sum = vec4(0); int n = 0;
	for(int v = -radius; v <= radius; ++v)
	{
		sum += fetch(0, v);
		n++;
	}
	result = sum / n;
}
