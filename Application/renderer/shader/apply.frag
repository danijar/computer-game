#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D effect_tex;
uniform sampler2D edge_tex;


void main()
{
	const int radius = 0;
	float blur = 0;
	int n = 0;

	vec3 color  = texture2D(image_tex,  coord).rgb;
	vec3 effect = texture2D(effect_tex, coord).rgb;

	image = vec4(color + vec3(0.3 * dot(effect, vec3(1)) * 0.5 - 0.5), 1.0);
	//image = vec4(vec3(0.3 * dot(effect, vec3(1))), 1.0); // only ssao
	//image = vec4(vec3(blur), 1.0);                       // only blurred ssao
	//image = vec4(color, 1.0);                            // only image
	//image = texture2D(edge_tex, coord);                  // only edge
}
