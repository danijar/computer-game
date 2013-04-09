#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D effect_tex;
uniform sampler2D edge_tex;
uniform vec2 frameBufSize;


float avg(vec3 value)
{
	return (value.x + value.y + value.z) / 3.0;
}

void main()
{
	const int radius = 3;
	float blur = 0;
	int n = 0;
	for(float u = -radius; u <= radius; ++u)
	for(float v = -radius; v <= radius; ++v)
	{
		vec2 coordinate = coord + vec2(u, v) / frameBufSize;
		vec3 edge = texture2D(edge_tex, coordinate).xyz;
		if(edge.x < 0.5 && edge.y < 0.5 && edge.z < 0.5)
		{
			blur += avg(texture2D(effect_tex, coordinate).rgb);
			n++;
		}
	}
	blur /= n;

	vec3 color = texture2D(image_tex, coord).rgb;
	const float ssao_amount = 0.5;

	image = vec4(color + blur * ssao_amount - ssao_amount, 1.0);
	//image = vec4(vec3(blur), 1.0);      // only ssao
	//image = vec4(color, 1.0);           // only image
	//image = texture2D(edge_tex, coord); // only edge
}
