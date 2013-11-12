#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;

// 0 means none, 1 is default, 2 is twice
uniform float saturation = 1.3;
uniform float red        = 1.0;
uniform float green      = 1.0;
uniform float blue       = 1.0;
uniform float brightness = 1.1;
uniform float contrast   = 1.1;

/*
 * useful parameter values
 *
 * default 1.3, 1.0, 1.0, 1.0, 1.1, 1.1
 * sunset  1.1, 2.0, 1.1, 0.5, 0.9, 1.0
 * crises  0.5, 0.7, 0.8, 1.1, 1.1, 1.3
 * night   0.4, 0.6, 0.9, 1.5, 1.4, 1.2
 */

void saturate(inout vec3 color, in float change)
{
	const float Pr = 0.299, Pg = 0.587, Pb = 0.114;
	float luminosity = Pr * color.r + Pg * color.g + Pb * color.b;

	color.r = luminosity + (color.r - luminosity) * change;
	color.g = luminosity + (color.g - luminosity) * change;
	color.b = luminosity + (color.b - luminosity) * change;
}

void detone(inout vec3 color, in vec3 modification)
{
	const float Pr = 0.299, Pg = 0.587, Pb = 0.114;

	float before = Pr * color.r + Pg * color.g + Pb * color.b;
	color *= modification;
	float after  = Pr * color.r + Pg * color.g + Pb * color.b;

	float change = after - before;
	color.r -= Pr * change;
	color.g -= Pg * change;
	color.b -= Pb * change;
}

void brighten(inout vec3 color, in float brightness, in float contrast)
{
	color *= vec3(brightness);

	float average = 0.5;
	vec3 offset = color - vec3(average);
	offset *= contrast;
	color = vec3(average) + offset;
}

void main()
{
	vec3 pixel = texture(image_tex, coord).rgb;

	saturate(pixel, saturation);
	detone(pixel, vec3(red, green, blue));
	brighten(pixel, brightness, contrast);

	pixel = clamp(pixel, 0, 1);
	image = vec4(pixel, 1.0);
}
