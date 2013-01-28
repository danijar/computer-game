#version 150

uniform sampler2D image;

in vec2 ftexcoord;

out vec4 frag_color;


void main()
{
	vec3 pixel = texture(image, ftexcoord).rgb;

	gl_FragColor = vec4(pixel, 1.0);
}
