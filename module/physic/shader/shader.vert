#version 330

in vec4 vertex;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * vec4(vertex.xyz, 1.0);
}
