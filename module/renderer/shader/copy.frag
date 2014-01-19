#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D copy;

void main()
{
	image = texture2D(copy, coord).rgb;
}
