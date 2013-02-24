#version 330 core

in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

out vec3 color;

uniform sampler2D tex;

void main()
{
	color = texture2D(tex, ftexcoord).rgb;
}
