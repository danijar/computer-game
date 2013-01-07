#version 150

in vec3 f_normal;
in vec2 f_texcoord;

uniform sampler2D tex;

// ambient
vec3 ambient_color = vec3(0.75, 0.65, 0.50);
float ambient_amount = 0.2;

// light
vec3 light_pos = vec3(0.5, 1.0, 1.5);
float light_amount = 0.5;

// fog
vec4 fog_color = vec4(0.4, 0.6, 0.9, 1.0);
int fog_distance = 200;

void main()
{
	vec4 pixel = texture(tex, f_texcoord);

	// ambient
	pixel = pixel + vec4(ambient_color * ambient_amount, 1.0);

	// lighting
	vec3 light = vec3(max(0.0, dot(normalize(f_normal), normalize(light_pos))));
    pixel = pixel * vec4(light * light_amount + (1 - light_amount), 1.0);

	// fog
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog_amount = clamp(pow(z / fog_distance, 3), 0, 1);
	pixel = mix(pixel, fog_color, fog_amount);

    gl_FragColor = pixel;
}