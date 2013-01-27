#version 150

uniform sampler2D tex;

in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

out vec3 position;
out vec3 normal;
out vec3 albedo;

void main()
{
	position   = fposition;
	normal.xyz = normalize(fnormal.xyz);
	albedo.rgb = vec3(texture(tex, ftexcoord));
}
