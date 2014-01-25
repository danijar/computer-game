#version 330

in vec2 coord;
out vec3 image;

uniform int type = 0;

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D speculars;
uniform vec3 light;
uniform vec3 color;
uniform float radius;
uniform float intensity = 1.0;

void main()
{
	// light types
	bool directional = (type == 0);
	bool point       = (type == 1);
	bool spot        = (type == 2);

	// samplers
	vec3  normal    = texture2D(normals,   coord).xyz;
	vec3  position  = texture2D(positions, coord).xyz;
	float shininess = texture2D(speculars, coord).x;

	// normalize directional light source
	vec3 source;
	if(directional) source = position + normalize(light);
	else source = light;

	// check direction
	vec3 arriving = normalize(source - position);
	float faced = dot(arriving, normal);
	
	// reflection
	vec3 lookat = normalize(position);
	float reflection = max(0, dot(reflect(arriving, normal), lookat));
	float specular = shininess * pow(reflection, pow(10, shininess - 1));
	
	// this was the previous equation
	// vec3 lookat = vec3(0, 0, -1);
	// float reflection = max(0, dot(reflect(arriving, normal), lookat));
	// float specular = min(shininess, 1) * pow(reflection, shininess);

	// parameters
	float attenuation = 1;
	float fraction = 1;

	// light types
	if(directional) {
		fraction = clamp(faced / 2.0 + 0.5, 0, 1);
	}
	else if(point) {
		float away = max(distance(position, light), 0);
		float magnitude = 1 / pow(distance(position, light) / radius + 1, 2);
		float cutoff = 0.4;
		attenuation = (magnitude - cutoff) / (1 - cutoff);
		fraction = 0.8 * clamp(dot(normalize(light - position), normal), 0, 1) + 0.2;
		// show radius
		// attenuation = attenuation < 0.5 ? 0 : 1;
	}
	else if(spot) {
		// not implemented yet
	}

	// clamp values
	specular    = clamp(specular,    0, 1);
	attenuation = clamp(attenuation, 0, 1);
	fraction    = clamp(fraction,    0, 1);

	image = color * attenuation * intensity * (fraction + specular);
}
