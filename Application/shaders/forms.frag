#version 410

layout(location = 0) in vec3 fposition;
layout(location = 1) in vec3 fnormal;
layout(location = 2) in vec2 ftexcoord;

out vec4 position;
out vec4 normal;
out vec4 albedo;

uniform sampler2D tex;

void main()
{
	position.xyz = fposition;
	normal.xyz   = normalize(fnormal);
	albedo.rgba  = texture2D(tex, ftexcoord);
}
