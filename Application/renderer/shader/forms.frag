#version 330

in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 albedo;

uniform sampler2D tex;

void main()
{
	position = fposition;
	normal   = normalize(fnormal);
	albedo   = texture2D(tex, ftexcoord).rgb;
}
