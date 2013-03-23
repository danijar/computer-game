#version 330

in vec4 vertex;

out vec2 coord;

void main()
{
	coord = vertex.xy;
    gl_Position = vertex * 2.0 - 1.0;
}
