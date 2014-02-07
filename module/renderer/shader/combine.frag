#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D albedo;
uniform sampler2D lights;
uniform sampler2D depth;


void main()
{
	vec3 ambient = vec3(0.0); // later look this up from skymap
	vec3 color = texture2D(albedo, coord).rgb;
	vec3 light = texture2D(lights, coord).rgb;

	// do not light sky
	if (texture2D(depth, coord).r > 0.9999) { image = color; return; }

	// gray out in low light
	float grayout = clamp(0.75 - 0.2 * dot(light, vec3(1)), 0, 1);
	vec3 desaturated = vec3((color.r + color.g + color.b) / 3);
	// grayout = 0; // disable grayout

	image = ambient + mix(color, desaturated, grayout) * light;
	// image = ambient + light; // only light
	// image = color;           // only albedo
}
