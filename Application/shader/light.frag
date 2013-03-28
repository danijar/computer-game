#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D positions;
uniform sampler2D normals;
uniform vec3 light;
uniform vec3 color;
uniform float radius;
uniform float intensity = 1.0;


void main()
{
	vec3 pixel = texture2D(positions, coord).xyz;
	vec3 normal = texture2D(normals, coord).xyz;
	float dist = distance(pixel, light);
	float magnitude = clamp((radius - dist) / (0.5 * radius / intensity), 0, 1);
	float bias = 0.1;
	float fraction = (dot(normalize(light - pixel), normal) + 1.0) / dist;
	image.xyz = color * magnitude * fraction;
}
