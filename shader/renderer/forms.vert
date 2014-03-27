#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 fposition;
out vec3 fnormal;
out vec2 fcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec4 round(vec4 value, float prec)
{
	return floor(value / prec + 0.5) * prec;
}

vec3 noise(vec3 value)
{
	float x = fract(sin(dot(value, vec3(12.9898, 78.2330, 34.5048))) * 43758.5453);
	float y = fract(sin(dot(value, vec3(66.8461, 24.6561, 70.8345))) * 52217.4699);
	float z = fract(sin(dot(value, vec3(60.5442, 97.9837, 36.2911))) * 60209.9005);
	return vec3(x, y, z);
}

void main()
{
	vec3 location = position;

	// skew vertices
	// location = position + 0.3 * noise(position);

	fposition = vec3(view * model * vec4(location, 1));
	fnormal = vec3(normalize(view * model * vec4(normal, 0)));
	fcoord = texcoord;
	gl_Position = projection * view * model * vec4(location, 1);

	// snap to world grid
	// gl_Position = projection * view * round(model * vec4(location, 1.0), 0.25);

	// snap to screen grid
	// gl_Position = round(gl_Position, 1);
}
