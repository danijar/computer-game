#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D albedo;
uniform sampler2D lights;


void main()
{
	vec3 ambient = vec3(0); // later look this up from skymap
	image = ambient + texture2D(albedo, coord).rgb * texture2D(lights, coord).rgb;
	// image = ambient + texture2D(lights, coord).rgb; // only light
	// image = texture2D(albedo, coord).rgb;           // only albedo
}
