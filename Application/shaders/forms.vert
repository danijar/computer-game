#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec3 fposition;
out vec3 fnormal;
out vec2 ftexcoord;

void main()
{
	mat4 mvp = projection * view * model;

	fposition   =      vec3(mvp   * vec4(position, 1.0));
	fnormal     = vec3(normalize(model * vec4(normal,   0.0)));
	ftexcoord   = texcoord;
	gl_Position = mvp * vec4(position, 1.0);
}
