#version 150

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

const vec3 light_position = vec3(0.0, 0.0, 0.0);
const vec3 light_color = vec3(1.0);
const float ambient_factor = 0.2;
const float shininess = 50.0;

in vec2 ftexcoord;

out vec4 frag_color;

void main()
{
	// Read in the eye position, normal, and albedo from the
	// g-buffer.
	vec4 sample = texture(position_tex, ftexcoord);  
	vec3 position = sample.rgb;

	sample = texture(normal_tex, ftexcoord);  
	vec3 normal = sample.rgb;

	sample = texture(albedo_tex, ftexcoord);  
	vec3 albedo = sample.rgb;

	// And now we light like normal.
	vec3 l = normalize(light_position - position);
	float n_dot_l = max(dot(normal, l), 0.0);

	vec3 v = normalize(-position);
	vec3 h = normalize(v + l);
	float n_dot_h = max(dot(normal, h), 0.0);

	vec3 ambient = ambient_factor * light_color * albedo;

	vec3 diffuse = (1.0 - ambient_factor) * n_dot_l * light_color * albedo;
	vec3 specular = pow(n_dot_h, shininess) * light_color;

	vec3 fcolor = specular + diffuse + ambient;
	frag_color = vec4(fcolor, 1.0);
}
