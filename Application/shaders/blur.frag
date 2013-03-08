#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform vec2 frameBufSize = vec2(800, 600);

void main()
{
	const int radius = 3;
	vec2 step = 1.0 / frameBufSize;

	vec4 pixel = vec4(0);
	int n = 0;
	for(float u = -radius*step.x; u <= +radius*step.x; u += step.x)
	for(float v = -radius*step.y; v <= +radius*step.y; v += step.y)
	{
		pixel += texture2D(image_tex, coord + vec2(u, v));
		n++;
	}
	pixel /= n;

	image = vec4(pixel.xyz, 1.0);
}
