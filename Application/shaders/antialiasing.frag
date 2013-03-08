#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform vec2 frameBufSize = vec2(800, 600);

void main()
{
	if(texture2D(image_tex, coord).a == 0.0) discard;

	vec2 step = 1.0 / frameBufSize;

	float dc = texture2D(position_tex, coord).z;
	float dd[9];

	vec3 nc = texture2D(normal_tex, coord).xyz;
	float nd[9];

	int i = 0;
	for(int u = -1; u <= +1; ++u)
	for(int v = -1; v <= +1; ++v)
	{
		dd[i] =     abs(    dc - texture2D(position_tex, coord + vec2(u, v) * step).z   );
	    nd[i] = 1 - abs(dot(nc,  texture2D(normal_tex,   coord + vec2(u, v) * step).xyz));
		i++;
	}

	float da = 0, na = 0;
	for(int j = 0; j < i; ++j)
	{
		da += dd[j];
		na += nd[j];
	}
	da /= i;
	na /= i;

	float amount = 2 * mix(da, na, 0.8);

	const int radius = 1;
	vec3 blur = vec3(0);
	int n = 0;
	for(float u = -radius; u <= +radius; ++u)
	for(float v = -radius; v <= +radius; ++v)
	{
		blur += texture2D(image_tex, coord + vec2(u, v) * step).rgb;
		n++;
	}
	blur /= n;

	vec3 color = texture2D(image_tex, coord).rgb;
	image = vec4(mix(color, blur, min(amount, 1)), 1.0);
}
