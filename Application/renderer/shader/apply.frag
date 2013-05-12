#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D image_tex;
uniform sampler2D effect_tex;


void main()
{
	const int radius = 0;
	float blur = 0;
	int n = 0;

	vec3 color  = texture2D(image_tex,  coord).rgb;
	vec3 effect = texture2D(effect_tex, coord).xyz;

	image = color + vec3(0.3 * dot(effect, vec3(1)) * 0.5 - 0.5);
	//image = vec3(0.3 * dot(effect, vec3(1))); // only ssao
	//image = color;                            // only image
}
