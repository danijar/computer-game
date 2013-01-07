#version 150

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec2 f_texcoord;
out vec4 f_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

vec3 light = vec3(0.5, 1.0, 1.5);
float amount = 0.6;

void main()
{
    gl_Position = proj * view * model * vec4(position, 1.0);

    f_texcoord = texcoord;

	vec3 diffuse = max(0.0, dot(normalize(normal), normalize(light)));
    f_color = vec4(diffuse, 1.0) * amount + (1-amount);
}