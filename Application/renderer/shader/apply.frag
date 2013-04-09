#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D effect_tex;
uniform vec2 frameBufSize = vec2(1000);


float avg(vec3 value)
{
	return (value.x + value.y + value.z) / 3.0;
}

void main()
{
	const int radius = 1;
	float blur = 0;
	int n = 0;
	for(float u = -radius; u <= +radius; ++u)
	for(float v = -radius; v <= +radius; ++v)
	{
		blur += avg(texture2D(effect_tex, coord + vec2(u, v) / frameBufSize).rgb);
		n++;
	}
	blur /= n;

	vec3 color = texture2D(image_tex, coord).rgb;
	const float ssao_amount = 0.3;
	image = vec4(color + blur * ssao_amount - ssao_amount, 1.0); // combine
	//image = vec4(vec3(blur), 1.0);                             // display only
	//image = vec4(color, 1.0);                                  // skip ssao
}
