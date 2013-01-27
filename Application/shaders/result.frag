#version 150

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

in vec2 ftexcoord;

out vec4 frag_color;


// ambient
vec3 ambient_color = vec3(0.75, 0.65, 0.50);
float ambient_amount = 0.2;

// lighting
vec3 light_pos = vec3(0.5, 1.0, 1.5);
float light_amount = 0.5;

// fog
vec4 fog_color = vec4(0.4, 0.6, 0.9, 1.0);
int fog_distance = 200;

void main()
{
	if(texture(albedo_tex, ftexcoord).a == 0.0) discard;

	vec3 pixel = vec3(0);

	vec3 position = texture(position_tex, ftexcoord).xyz;
	vec3 normal = texture(normal_tex, ftexcoord).xyz;
	vec3 albedo = texture(albedo_tex, ftexcoord).rgb;

	float depth = position.z / 25;
	

	// texture
	pixel += albedo;

	// ambient
	pixel += ambient_amount * ambient_color;

	// lighting
	vec3 light = vec3(max(0.0, dot(normalize(normal), normalize(light_pos))));
    pixel *= light_amount/2 + light;

	// fog
	// float fog_amount = clamp(pow(depth / fog_distance, 3), 0, 1);
	// pixel = mix(pixel, fog_color, fog_amount);

    gl_FragColor = vec4(pixel, 1.0);

	// gl_FragColor = vec4(vec3(1 - depth), 1.0); // display depth
}
