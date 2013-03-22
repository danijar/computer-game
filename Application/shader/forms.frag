#version 410

in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 albedo;

uniform sampler2D tex;

void main()
{
	vec2 texcoord = vec2(ftexcoord.x, 1.0 - ftexcoord.y); // flip texcoords

	position.xyz = fposition;
	normal.xyz   = normalize(fnormal);
	albedo.rgba  = texture2D(tex, texcoord);
}
