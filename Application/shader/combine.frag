#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D albedo;
uniform sampler2D lights;


void main()
{
	vec3 ambient = vec3(0); // later on look this up from skymap
    image = vec4(ambient, 0.0) + texture2D(albedo, coord) * texture2D(lights, coord);
}
