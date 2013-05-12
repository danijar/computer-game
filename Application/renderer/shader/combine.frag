#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D albedo;
uniform sampler2D lights;
uniform sampler2D position;


void main()
{
	vec3 ambient = vec3(0.0); // later look this up from skymap
	vec3 color = texture2D(albedo, coord).rgb;

	image = ambient + color * texture2D(lights, coord).rgb;
	// image = ambient + texture2D(lights, coord).rgb; // only light
	// image = texture2D(albedo, coord).rgb;           // only albedo

	// do apply lighting to sky dome
	if(1 - texture2D(position, coord).z > 700) image = color;
}
