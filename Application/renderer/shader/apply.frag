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
	const int radius = 5;
	float blur = avg(texture2D(effect_tex, coord).rgb);
	int n = 1;

	for(float u = -radius; u < 0; ++u)
	{
		vec2 coordinate = coord + vec2(u, 0) / frameBufSize;

		if(dot(texture2D(edge_tex, coordinate).xyz, vec3(1)) > 0.5) break;

		blur += avg(texture2D(effect_tex, coordinate).rgb);
		n++;
	}
	for(float u = 1; u <= radius; ++u)
	{
		vec2 coordinate = coord + vec2(u, 0) / frameBufSize;

		if(dot(texture2D(edge_tex, coordinate).xyz, vec3(1)) > 0.5) break;

		blur += avg(texture2D(effect_tex, coordinate).rgb);
		n++;
	}
	for(float v = -radius; v < 0; ++v)
	{
		vec2 coordinate = coord + vec2(0, v) / frameBufSize;
		vec3 edge = texture2D(edge_tex, coordinate).xyz;

		if(dot(texture2D(edge_tex, coordinate).xyz, vec3(1)) > 0.5) break;

		blur += avg(texture2D(effect_tex, coordinate).rgb);
		n++;
	}
	for(float v = 1; v <= radius; ++v)
	{
		vec2 coordinate = coord + vec2(0, v) / frameBufSize;
		vec3 edge = texture2D(edge_tex, coordinate).xyz;

		if(dot(texture2D(edge_tex, coordinate).xyz, vec3(1)) > 0.5) break;

		blur += avg(texture2D(effect_tex, coordinate).rgb);
		n++;
	}
	blur /= n;

	vec3 color = texture2D(image_tex, coord).rgb;
	const float ssao_amount = 0.5;

	image = vec4(color + vec3(blur * ssao_amount - ssao_amount), 1.0);
	//image = vec4(vec3(blur), 1.0);      // only ssao
	//image = vec4(color, 1.0);           // only image
	//image = texture2D(edge_tex, coord); // only edge
}
