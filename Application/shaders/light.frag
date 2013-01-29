#version 150

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

in vec2 ftexcoord;

out vec4 image;


void main()
{
	if(texture(albedo_tex, ftexcoord).a == 0.0) discard;

	vec3 pixel = vec3(0);
	vec3 position = texture(position_tex, ftexcoord).xyz;
	vec3 normal = texture(normal_tex, ftexcoord).xyz;
	vec3 albedo = texture(albedo_tex, ftexcoord).rgb;

	const float max_depth = 100.0;
	float depth = min(position.z / max_depth, max_depth);

	// texture
	pixel += albedo;

	// ambient
	const vec3 ambient_color = vec3(0.75, 0.65, 0.50);
	const float ambient_amount = 0.2;
	pixel += ambient_amount * ambient_color;

	// lighting
	const vec3 light_pos = vec3(0.5, 1.0, 1.5);
	const float light_amount = 0.5;
	vec3 light = vec3(max(0.0, dot(normalize(normal), normalize(light_pos))));
	pixel *= light_amount/2 + light;

	// fog
	const vec3 fog_color = vec3(0.4, 0.6, 0.9);
	const int fog_distance = 100;
	float fog_amount = 2 * pow(depth, 2) / fog_distance;
	pixel = mix(pixel, fog_color, min(1, fog_amount));

	image = vec4(pixel, 1.0);             // default
	//image = vec4(normal, 1.0);          // normals
	//image = vec4(vec3(1 - depth), 1.0); // depth

}
