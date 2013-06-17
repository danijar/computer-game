#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D image_tex;
uniform sampler2D effect_tex;
uniform float amount = 0.5;


void main()
{
	vec3 color  = texture2D(image_tex,  coord).rgb;
	float effect = texture2D(effect_tex, coord).x;

	image = color - vec3(amount - amount * effect);
	// image = vec3(effect); // only ssao
	// image = color;        // only image
}
