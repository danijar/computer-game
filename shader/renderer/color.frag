#version 330

in vec2 coord;
out vec3 image;

uniform vec3 color;

void main()
{
	image = color;
}
