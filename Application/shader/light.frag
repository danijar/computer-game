#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

void main()
{
	if(texture(albedo_tex, coord).a == 0.0) discard;

	vec3 pixel = vec3(0);
	vec3 position = texture(position_tex, coord).xyz;
	vec3 normal = texture(normal_tex, coord).xyz;
	vec3 albedo = texture(albedo_tex, coord).rgb;

	const float max_depth = 100.0;
	float depth = min(position.z / max_depth, max_depth);

	// texture
	pixel += albedo;

	// ambient
	const vec3 ambient_color = vec3(0.27, 0.23, 0.13);
	const float ambient_amount = 0.3;
	pixel = mix(pixel, (pixel + ambient_color) / 2, ambient_amount);

	// lighting
	const vec3 light_pos = vec3(0.5, 1.0, 1.5);
	const float light_amount = 0.6;
	vec3 light = vec3(max(0.0, dot(normalize(normal), normalize(light_pos))));
	pixel = mix(pixel, 2 * pixel * light, light_amount);

	// fog
	const vec3 fog_color = vec3(0.4, 0.6, 0.9);
	const int fog_distance = 100;
	float fog_amount = 2 * pow(depth, 2) / fog_distance;
	pixel = mix(pixel, fog_color, min(1, fog_amount));

	image = vec4(pixel, 1.0);             // default
	//image = vec4(normal, 1.0);          // normals
	//image = vec4(vec3(1 - depth), 1.0); // depth
}
