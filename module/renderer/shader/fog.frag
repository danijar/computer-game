#version 330

in vec2 coord;
out vec3 result;

uniform sampler2D colors;
uniform sampler2D depths;
uniform sampler2D lights;

uniform float dismiss = 0.1;


void main()
{
	vec3 color = texture2D(colors, coord).rgb;
	float depth = texture2D(depths, coord).r;
	vec3 light = texture2D(lights, coord).rgb;

	// should be done by stencil
	if (depth > 0.9999) { result = color; return; }

	vec3 fog = vec3(0.5, 0.6, 0.7);
	float amount = clamp(pow(depth / dismiss, 3), 0, 0.6);

	result = mix(color, fog, amount);
}
