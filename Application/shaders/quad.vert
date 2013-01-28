#version 150

in vec2 position;
in vec2 texcoord;

out vec2 ftexcoord;

void main()
{
	ftexcoord   = texcoord;
	gl_Position = vec4(position, 0.0, 1.0);
}
