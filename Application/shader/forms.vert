#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 fposition;
out vec3 fnormal;
out vec2 ftexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 mvp = projection * view * model;

	fposition = vec3(view * model * vec4(position, 1.0));
	fnormal = vec3(normalize(mvp * vec4(normal, 0.0)));
	ftexcoord = texcoord;
	gl_Position = mvp * vec4(position, 1.0);
}
