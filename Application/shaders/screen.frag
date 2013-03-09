#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;

void main()
{
	vec3 pixel = texture(image_tex, coord).rgb;

	image = vec4(pixel, 1.0);
}
