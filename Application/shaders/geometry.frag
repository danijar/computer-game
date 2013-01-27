#version 150

uniform sampler2D tex;

in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

out vec4 position;
out vec4 normal;
out vec4 albedo;

void main()
{
	position    = vec4(fposition, 0.5);
	normal.xyz  = normalize(fnormal);
	albedo.rgba = texture(tex, ftexcoord);
}
